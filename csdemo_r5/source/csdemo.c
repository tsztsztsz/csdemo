/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "platform.h"

#include "csaccess.h"
#include "cs_utility.h"
#include "cs_demo_known_boards.h"

/* The CS register specification is needed only for the filter details in
 ETM/PTM configuration - this should be abstracted out somehow (TBD) */
#include "csregisters.h"

#include "cs_trace_metadata.h"

#include <sched.h>		/* for CPU_* family, requires glibc 2.6 or later */
#include <unistd.h>		/* for usleep() */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>


/* Change these to capture the specific region of the kernel required
 KERNEL_TRACE_SIZE          is the extent of the region to trace
 KERNEL_TRACE_VIRTUAL_ADDR  is the virtual address of the start of the region to trace,
 i.e. corresponding to addresses in the vmlinux file
 */
#define ALL_CPUS -1

static struct cs_devices_t devices;

/* command line options */
static int cpu_to_trace;
static bool itm;
static bool itm_only;
static bool full;
static bool verbose;
static bool trace_timestamps;
static bool trace_cycle_accurate;
#define BOARD_NAME_LEN 256
static char board_name[BOARD_NAME_LEN];

static bool etb_stop_on_flush;
static unsigned int etb_post_trig_words;

static bool return_stack;

#define INVALID_ADDRESS 1	/* never a valid address */
static unsigned long o_trace_start_address = INVALID_ADDRESS;
static unsigned long o_trace_end_address = INVALID_ADDRESS;




/* Pause after each significant step in the demo */
int pause_mode;

void pause_demo(void) {
	fflush(stdout);
	if (pause_mode) {
		int c;
		fprintf(stderr, "[press RETURN to continue or q/Q to quit]\r");
		fflush(stderr);
		c = getchar();
		if (c == 'q' || c == 'Q') {
			fprintf(stderr, "CSDEMO: exiting demo.\n");
			exit(EXIT_FAILURE);
		}
	} else {
		fflush(stderr);
	}
}

void await_user_stop(void) {
	int c;
	fprintf(stderr, "[press RETURN to stop tracing or q/Q to quit]\r");
	fflush(stderr);
	c = getchar();
	if (c == 'q' || c == 'Q') {
		fprintf(stderr, "CSDEMO: exiting demo.\n");
		exit(EXIT_FAILURE);
	}
}

void fetch_trace_option(void) {
	int c;
	fprintf(stderr, "[press RETURN to fetch trace data or q/Q to quit]\r");
	fflush(stderr);
	c = getchar();
	if (c == 'q' || c == 'Q') {
		fprintf(stderr, "CSDEMO: exiting demo.\n");
		cs_shutdown();
		exit(EXIT_FAILURE);
	}
}

static int do_init_etm(cs_device_t dev) {
	int rc;
	struct cs_etm_config config;

	printf("CSDEMO: Initialising ETM/PTM\n");

	/* ASSERT that this is an etm etc */
	assert(cs_device_has_class(dev, CS_DEVCLASS_SOURCE));

	/* set to a 'clean' state - clears events & values, retains ctrl and ID, ensure programmable */
	if ((rc = cs_etm_clean(dev)) != 0) {
		printf("CSDEMO: Failed to set ETM/PTM into clean state\n");
		return rc;
	}

	/* program up some basic trace control.
	 Set up to trace all instructions.
	 */
	/* ETMv4 initialisation */
	cs_etmv4_config_t v4config;

	cs_etm_config_init_ex(dev, &v4config);
	v4config.flags = CS_ETMC_CONFIG;
	cs_etm_config_get_ex(dev, &v4config);
	v4config.flags |= CS_ETMC_TRACE_ENABLE | CS_ETMC_EVENTSELECT;
	/* trace enable */
	if (itm_only) {
		printf("No Viewinst, ITM only\n");
		v4config.victlr = 0x0; /* Viewinst - trace nothing. */
	} else {
		printf("Viewinst trace everything\n");
		v4config.victlr = 0x201; /* Viewinst - trace all, ss started. */
	}
	v4config.viiectlr = 0; /* no address range */
	v4config.vissctlr = 0; /* no start stop points */
	/* event select */
	v4config.eventctlr0r = 0; /* disable all event tracing */
	v4config.eventctlr1r = 0;
	/* config */
	v4config.stallcrlr = 0; /* no stall */
	v4config.syncpr = 0xC; /* sync 0c9=512 bytes; 0xC=4096 bytes */
	cs_etm_config_put_ex(dev, &v4config);

	return 0;
}

static void show_etm_config(unsigned int n) {
	cs_etm_config_t tconfig; /* PTM/ETMv3 config */
	cs_etmv4_config_t t4config; /* ETMv4 config */
	void *p_config = 0;

	if (CS_ETMVERSION_MAJOR(cs_etm_get_version(devices.etm[n])) >=
	CS_ETMVERSION_ETMv4)
		p_config = &t4config;
	else
		p_config = &tconfig;

	cs_etm_config_init_ex(devices.etm[n], p_config);
	tconfig.flags = CS_ETMC_ALL;
	t4config.flags = CS_ETMC_ALL;
	cs_etm_config_get_ex(devices.etm[n], p_config);
	cs_etm_config_print_ex(devices.etm[n], p_config);
}


static int do_config_etmv4(int n_core) {
	cs_etmv4_config_t tconfig;
	cs_device_t etm = devices.etm[n_core];

	/* default settings are trace everything - already set. */
	cs_etm_config_init_ex(etm, &tconfig);
	tconfig.flags =
	CS_ETMC_TRACE_ENABLE | CS_ETMC_CONFIG | CS_ETMC_EVENTSELECT;
	cs_etm_config_get_ex(etm, &tconfig);

	if (tconfig.scv4->idr2.bits.vmidsize > 0)
		tconfig.configr.bits.vmid = 1; /* VMID trace enable */
	if (tconfig.scv4->idr2.bits.cidsize > 0)
		tconfig.configr.bits.cid = 1; /* context ID trace enable. */

	if (return_stack)
		tconfig.configr.bits.rs = 1; /* set the return stack */

	if (!full) {
		/*  set up an address range filter - use comparator pair and the view-inst registers */

		tconfig.addr_comps[0].acvr_l = o_trace_start_address & 0xFFFFFFFF;
		tconfig.addr_comps[0].acvr_h = (o_trace_start_address >> 32) & 0xFFFFFFFF;
		tconfig.addr_comps[0].acatr_l = 0x0; /* instuction address compare, all ELs, no ctxt, vmid, data, etc */
		tconfig.addr_comps[1].acvr_l = o_trace_end_address & 0xFFFFFFFF;
		tconfig.addr_comps[1].acvr_h = (o_trace_end_address >> 32) & 0xFFFFFFFF;
		tconfig.addr_comps[1].acatr_l = 0x0; /* instuction address compare, all ELs, no ctxt, vmid, data, etc */

		/* mark the config structure to program the above registers on 'put' */
		tconfig.addr_comps_acc_mask = 0x3;
		tconfig.flags |= CS_ETMC_ADDR_COMP;

		/* finally, set up ViewInst to trace according to the resources we have set up */
		tconfig.viiectlr = 0x1; /* program the address comp pair 0 for include */
		tconfig.syncpr = 0x0; /* no extra sync */

	}
	cs_etm_config_print_ex(etm, &tconfig);
	cs_etm_config_put_ex(etm, &tconfig);

	/* Show the resulting configuration */
//	printf("CSDEMO: Reading back configuration after programming...\n");
//	show_etm_config(n_core);

	if (cs_error_count() > 0) {
		printf("CSDEMO: %u errors reported in configuration - not running demo\n", cs_error_count());
		return -1;
	}
	return 0;
}

static int do_configure_trace(const struct board *board) {
	int i, r;
	printf("*******************************************************\n");
	printf("CSDEMO: Configuring trace...\n");
	printf("*******************************************************\n");

	/*********************************************************/
	/* CORESIGHT SINKS - Disable TPIU / ETFs */

	/* Ensure TPIU isn't generating back-pressure */
	cs_disable_tpiu();
	/* While programming, ensure we are not collecting trace */
	cs_sink_disable(devices.etr);
	cs_sink_disable(devices.etf_main);
	cs_sink_disable(devices.etf_a53);
	if (devices.itm_etb != NULL) {
		cs_sink_disable(devices.itm_etb);
	}
	/*********************************************************/

	/*********************************************************/
	/* CORESIGHT SOURCES - Configure ETMs / STM */
	/*
	 * Setting Source ID for ETMs, Initialize ETMs (ViewInst, Sync, StartStop Range)
	 */
	//for (i = 0; i < board->n_cpu; ++i) {
		i=0;
		printf("CSDEMO: Configuring trace source id for CPU #%d ETM/PTM...\n", i);
		devices.etm[i] = cs_cpu_get_device(i, CS_DEVCLASS_SOURCE);
		if (devices.etm[i] == CS_ERRDESC) {
			fprintf(stderr, "** Failed to get trace source for CPU #%d\n", i);
			return -1;
		}
		if (cs_set_trace_source_id(devices.etm[i], 0x10 + i) < 0) {
			return -1;
		}
		if (do_init_etm(devices.etm[i]) < 0) {
			return -1;
		}
	//}
//	if (itm) {
//		cs_set_trace_source_id(devices.itm, 0x20);
//	}
	/*
	 * Configure ETMs (VMID, ContextID, Address Range Filter)
	 */
		r = do_config_etmv4(0);
		if (r != 0)
			return r;
	/*
	 * Enable timestamps and cycle accurate tracing,
	 */
	if (trace_timestamps)
		cs_trace_enable_timestamps(devices.etm[0], 1);
	if (trace_cycle_accurate)
		cs_trace_enable_cycle_accurate(devices.etm[0], 1);
	cs_checkpoint();
	/*********************************************************/

	/*
	 *  Enable ETF-main device for use in HW FIFO Mode
	 */
	printf("CSDEMO: Enabling ETF-Main in HW FIFO mode...\n");
	if (cs_sink_enable(devices.etf_main) != 0) {
		printf("CSDEMO: Could not enable trace buffer - not running demo\n");
		return -1;
	}
	/*
	 * Enable ETF-A53 device for use in HW FIFO Mode
	 */
	printf("CSDEMO: Enable ETF-A53 in HW FIFO mode...\n");
	if (cs_etf_enable(devices.etf_a53) != 0) {
		printf("CSDEMO: Could not enable trace FIFO - not running demo\n");
		return -1;
	}
	/*********************************************************/

	/*********************************************************/
	/* for this demo we may set stop on flush and stop capture by maunal flushing later */
	unsigned int ffcr_val;
	if (etb_stop_on_flush) {
		/* ETF8K */
		/* set up some bits in the FFCR - enabling the  ETB later will retain these bits */
		ffcr_val = cs_device_read(devices.etf_main, CS_ETB_FLFMT_CTRL);
		ffcr_val |= CS_ETB_FLFMT_CTRL_StopFl; // set StopOnFlush bit: the formatter is stopped on completion of a flush operation
		if (cs_device_write(devices.etf_main, CS_ETB_FLFMT_CTRL, ffcr_val) == 0) {
			printf("CSDEMO: setting stop on flush, ETB FFCR = 0x%08X\n", ffcr_val);
		} else {
			printf("CSDEMO: Failed to set stop on flush, ETB FFCR to 0x%08X\n", ffcr_val);
		}
		/* ETF4K */
		/* set up some bits in the FFCR - enabling the  ETB later will retain these bits */
		ffcr_val = cs_device_read(devices.etf_a53, CS_ETB_FLFMT_CTRL);
		ffcr_val |= CS_ETB_FLFMT_CTRL_StopFl; // set StopOnFlush bit: the formatter is stopped on completion of a flush operation
		if (cs_device_write(devices.etf_a53, CS_ETB_FLFMT_CTRL, ffcr_val) == 0) {
			printf("CSDEMO: setting stop on flush, ETB FFCR = 0x%08X\n", ffcr_val);
		} else {
			printf("CSDEMO: Failed to set stop on flush, ETB FFCR to 0x%08X\n", ffcr_val);
		}
	}
	/*********************************************************/

	/*********************************************************/
	/* Start ETM[0] */
	/*********************************************************/
	cs_trace_enable(devices.etm[0]); // FEC40000
	cs_checkpoint();
	/*********************************************************/


	if (cs_error_count() > 0) {
		printf("CSDEMO: %u errors reported when enabling trace - not running demo\n", cs_error_count());
		return -1;
	}

	/*
	printf("CSDEMO: CTI settings....\n");
	cs_cti_diag();
	*/
	printf("*******************************************************\n");
	printf("CSDEMO: Configured and enabled trace. [ETFs, ETMs, STM]\n");
	printf("*******************************************************\n");
	return 0;
}

/* if we have set up stop on flush, manual flush and wait for stop */
static void cs_etf_flush_and_wait_stop() {
	unsigned int ffcr_val, ffsr_val;
	// ETF4K [ETF]
	printf("CSDEMO: Flushing ETB and waiting for formatter stop\n");
	ffcr_val = cs_device_read(devices.etf_main, CS_ETB_FLFMT_CTRL);
	ffcr_val |= CS_ETB_FLFMT_CTRL_FOnMan;
	cs_device_write(devices.etf_main, CS_ETB_FLFMT_CTRL, ffcr_val);
	if (cs_device_wait(devices.etf_main, CS_ETB_FLFMT_STATUS, CS_ETB_FLFMT_STATUS_FtStopped, CS_REG_WAITBITS_ALL_1, 0, &ffsr_val) == 0) {
		if (verbose)
			printf("CSDEMO: ETB collection stopped\n");
	} else {
		if (verbose)
			printf("CSDEMO: ETB FFSR=0x%08X\n", ffsr_val);
		printf("CSDEMO: Warning ETB collection not stopped on flush on trigger\n");
	}
	// ETF8K [ETB]
	printf("CSDEMO: Flushing ETB and waiting for formatter stop\n");
	ffcr_val = cs_device_read(devices.etf_main, CS_ETB_FLFMT_CTRL);
	ffcr_val |= CS_ETB_FLFMT_CTRL_FOnMan; // set FLushMan bit: manually generate a flush of the system. [If TraceCaptEn= 0, writes to FlushMan are ignored.]
	cs_device_write(devices.etf_main, CS_ETB_FLFMT_CTRL, ffcr_val);
	if (cs_device_wait(devices.etf_main, CS_ETB_FLFMT_STATUS, CS_ETB_FLFMT_STATUS_FtStopped, CS_REG_WAITBITS_ALL_1, 0, &ffsr_val) == 0) {
		if (verbose)
			printf("CSDEMO: ETB collection stopped\n");
	} else {
		if (verbose)
			printf("CSDEMO: ETB FFSR=0x%08X\n", ffsr_val);
		printf("CSDEMO: Warning ETB collection not stopped on flush on trigger\n");
	}
}

int cs_manual_path_enable() {
	int rc;
	unsigned int fnl_ctrl;
	/*
	 * enable slave port 0 in Funnel FNL-A53
	 */
	printf("CSDEMO: Enable Path: ETM0 -> Port0 @ A53-Funnel \n");
	fnl_ctrl = cs_device_read(devices.fnl_a53, CS_FUNNEL_CTRL);
	fnl_ctrl |= CS_FUNNEL_EnSLV0;
	rc = cs_device_write(devices.fnl_a53, CS_FUNNEL_CTRL, fnl_ctrl);

	/*
	 * enable slave port 2 in Funnel FNL-MAIN
	 */
	printf("CSDEMO: Enable Path: ETF4K -> Port2 @ MAIN-Funnel \n");
	fnl_ctrl = cs_device_read(devices.fnl_main, CS_FUNNEL_CTRL);
	fnl_ctrl |= CS_FUNNEL_EnSLV2;
	rc = cs_device_write(devices.fnl_main, CS_FUNNEL_CTRL, fnl_ctrl);

	return rc;
}

int main(int argc, char **argv) {

    init_platform();

	strcpy(board_name,"Xilinx Zynq Ultrascale+ MPSoC");

	/*
	 * Trace Configuration
	 */
	cpu_to_trace = 0;        //ALL_CPUS;	// no CPU affinity selected (yet), trace all CPUs
	itm = false;
	itm_only = false;
	full = false; 			 // false: enable address range filter, true: enable filtering
	etb_stop_on_flush = 1;
	etb_post_trig_words = 0;
	trace_timestamps = false;
	trace_cycle_accurate = false;
	return_stack = false;
	cs_diag_set(1);
	verbose = true;
	pause_mode = 0;

	if(!full) {
		o_trace_start_address = 0x0000000000015a1c;
		o_trace_end_address = 0x0000000000015b2c;
	}

	const struct board *board;
	int i;
	printf("CoreSight demonstrator\n");

	/*****************************************************************/
	/* Registration of CoreSight Devices in local memory */
	/*****************************************************************/
	if (setup_known_board_by_name(board_name, &board, &devices) < 0) {
		return EXIT_FAILURE;
	}
	pause_demo();


	/*****************************************************************/
	/* Registration of CoreSight Devices in local memory */
	/*****************************************************************/
	bool run_tpiu_pattern_test = false;
	if(run_tpiu_pattern_test) {
		printf("*****************************\n");
		printf("CSDEMO: Run TPIU pattern test\n");
		printf("*****************************\n");
		if (cs_tpiu_pattern_test(devices.tpiu) < 0) {
			return EXIT_FAILURE;
		}
		// TBD: bypass, formatting, continuous formatting
		cs_checkpoint();
		cs_sink_disable(devices.tpiu);
	}
	/*****************************************************************/

	/*****************************************************************/
	/* Configuration of all CoreSight Devices */
	/*****************************************************************/
	/* Enable Slave Ports of Funnel Devices */
	if (cs_manual_path_enable() < 0) {
		return -1;
	}

	if (do_configure_trace(board) < 0) {
		return EXIT_FAILURE;
	}
	/*****************************************************************/

	/*****************************************************************/
	/* dump configuration of current setup [snapshot.ini, trace.ini, device_i.ini, cpu_i.ini, kernel_dump.bin] */
	/*****************************************************************/
	set_kernel_trace_dump_range(0x0000000000015a1c, 0x0000000000015b2c);
//	do_dump_config_uart(board, &devices, itm);



	await_user_stop();

	/*****************************************************************/
	/* Trace Session is active*/
	/* .... */
	/* run application to trace on A-53 */
	/* .... */
	/*****************************************************************/


	/*****************************************************************/
	/* Disable Trace Session */
	/*****************************************************************/


	/* Manual Flush / Formatter Stop of ETF-A53 & ETF-Main */
	/* Stop collection of trace data by manual flush and stop of formatter.
	 * This prevents the subsequent tracing of the disable code from overwriting interesting trace.
	 * Flush will also flush upstream devices such as the ETM/PTMs and STM/ITM */
	if (etb_stop_on_flush) {
		cs_etf_flush_and_wait_stop();
	}


	printf("CSDEMO: Disable trace...\n");
	/* now shut down all the sources */
	//for (i = 0; i < board->n_cpu; ++i) {
		i=0;
		cs_trace_disable(devices.etm[i]);
	//}

	cs_sink_disable(devices.etf_a53);
	cs_sink_disable(devices.etf_main);
//	cs_sink_disable(devices.tpiu);
	pause_demo();


	/*
	 * Enable fetch of trace data (Output via UART)
	 */
	fetch_trace_option();
	do_fetch_trace_etb_uart(devices.etf_main);

	printf("CSDEMO: shutdown...\n");
	cs_shutdown();

    cleanup_platform();
    return EXIT_SUCCESS;
}

/* end of tracedemo.c */
