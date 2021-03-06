/*
 CoreSight known board registration for demos
 Provides an abstract interface to registering boards with the library (for multiple demos)

 Copyright (C) ARM Limited, 2013-2016. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "cs_demo_known_boards.h"

#include <stdio.h>
#include <stdlib.h>

static int do_registration_zynqus(struct cs_devices_t *devices) {
	enum {
		A53_0, A53_1, A53_2, A53_3, R5_0, R5_1
	};
	cs_device_t rep, etr, etf_a53, etf_main, fun_main, fun_a53, fun_r5, stm, tpiu;
	cs_device_t cti0;//, cti1, cti2, cti0_a53, cti1_a53, cti2_a53, cti3_a53, cti0_r5, cti1_r5;
	int i;

	if (registration_verbose)
		printf("CSDEMO: Registering CoreSight devices...\n");
	cs_exclude_range(0xFEBF0000,0xFEBF7FFF);	// exclude R5-DBG0, R5-DBG1
	cs_register_romtable(0xFE800000);

	if (registration_verbose)
		printf("CSDEMO: Registering CPU affinities...\n");

	/* CTI affinities */
	cs_device_set_affinity(cs_device_register(0xFEBF8000), R5_0);
	cs_device_set_affinity(cs_device_register(0xFEBF9000), R5_1);
	cs_device_set_affinity(cs_device_register(0xFEC20000), A53_0);
	cs_device_set_affinity(cs_device_register(0xFED20000), A53_1);
	cs_device_set_affinity(cs_device_register(0xFEE20000), A53_2);
	cs_device_set_affinity(cs_device_register(0xFEF20000), A53_3);

	/* PMU affinities */
	// R5_0 - not available
	// R5_1 - not available
	cs_device_set_affinity(cs_device_register(0xFEC30000), A53_0);
	cs_device_set_affinity(cs_device_register(0xFED30000), A53_1);
	cs_device_set_affinity(cs_device_register(0xFEE30000), A53_2);
	cs_device_set_affinity(cs_device_register(0xFEF30000), A53_3);

	/* ETMv4 affinities */
	cs_device_set_affinity(cs_device_register(0xFEBFC000), R5_0);
	cs_device_set_affinity(cs_device_register(0xFEBFD000), R5_1);
	cs_device_set_affinity(cs_device_register(0xFEC40000), A53_0);
	cs_device_set_affinity(cs_device_register(0xFED40000), A53_1);
	cs_device_set_affinity(cs_device_register(0xFEE40000), A53_2);
	cs_device_set_affinity(cs_device_register(0xFEF40000), A53_3);

	if (registration_verbose)
		printf("CSDEMO: Registering trace-bus connections...\n");

	/*common setup */
	fun_main = cs_device_get(0xFE930000);
	etf_a53 = cs_device_get(0xFE940000);
	etf_main = cs_device_get(0xFE950000);
	rep = cs_device_get(0xFE960000);
	etr = cs_device_get(0xFE970000);
	tpiu = cs_device_get(0xFE980000);
	stm = cs_device_get(0xFE9C0000);

	/* connect A53 cores to funnel4p1 */
	fun_a53 = cs_device_get(0xFE920000);
	cs_atb_register(cs_cpu_get_device(A53_0, CS_DEVCLASS_SOURCE), 0, fun_a53, 0);
	cs_atb_register(fun_a53, 0, etf_a53, 0);
	cs_atb_register(etf_a53, 0, fun_main, 2);
	cs_atb_register(fun_main, 0, etf_main, 0);
	cs_atb_register(etf_main, 0, rep, 0);
	cs_atb_register(rep, 0, etr, 0);
	cs_atb_register(rep, 1, tpiu, 0);

	/* populate the devices structure */
	// etm/ptm registered in do_configure_trace()
	devices->itm = stm;
	devices->etf_a53 = etf_a53;
	devices->tpiu = tpiu;
	devices->etf_main = etf_main; /* core output through main etf */
	devices->etr = etr;

	// TBD: STM, FTM, CTI
	/* STM needs to init master address and master 0 by default
	All Juno cores see a single master @ 0, but other select bits
	ensure different cores and security options result in different
	master IDs in output.
	*/
	cs_stm_config_master(stm, 0, 0xF8000000);
	cs_stm_select_master(stm, 0);

	/* Connect system CTI to devices */
	cti0 = cs_device_register(0xFE990000);
	cs_cti_connect_trigsrc(etf_main, CS_TRIGOUT_ETB_FULL, cs_cti_trigsrc(cti0, 0));
	cs_cti_connect_trigsrc(etf_main, CS_TRIGOUT_ETB_ACQCOMP, cs_cti_trigsrc(cti0, 1));
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 7), etf_main,
	CS_TRIGIN_ETB_FLUSHIN);
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 6), etf_main,
	CS_TRIGIN_ETB_TRIGIN);

	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 0), tpiu,
	CS_TRIGIN_TPIU_FLUSHIN);
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 1), tpiu,
	CS_TRIGIN_TPIU_TRIGIN);

	cs_cti_connect_trigsrc(stm, CS_TRIGOUT_STM_TRIGOUTSPTE, cs_cti_trigsrc(cti0, 4));
	cs_cti_connect_trigsrc(stm, CS_TRIGOUT_STM_TRIGOUTSW, cs_cti_trigsrc(cti0, 5));
	cs_cti_connect_trigsrc(stm, CS_TRIGOUT_STM_TRIGOUTHETE, cs_cti_trigsrc(cti0, 6));
	cs_cti_connect_trigsrc(stm, CS_TRIGOUT_STM_ASYNCOUT, cs_cti_trigsrc(cti0, 7));

	/* edges of the CTI outputs are connected to separate HW events in STM */
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 4), stm,
	CS_TRIGIN_STM_HWEVENT_0); /* rising edge */
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 4), stm,
	CS_TRIGIN_STM_HWEVENT_1); /* falling edge */
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 5), stm,
	CS_TRIGIN_STM_HWEVENT_2); /* rising edge */
	cs_cti_connect_trigdst(cs_cti_trigdst(cti0, 5), stm,
	CS_TRIGIN_STM_HWEVENT_3); /* falling edge */


	/* primary part number from Main ID Register (MIDR) */
	for (i = 0; i < 4; i++)
		devices->cpu_id[i] = 0xD03; // CORTEX-A53
	for (i = 4; i < 6; i++)
		devices->cpu_id[i] = 0xC15; // CORTEX-R5

	return 0;
}

const struct board known_boards[] =
{
		{	.do_registration = do_registration_zynqus,
			.n_cpu = 6,
			.hardware = "Xilinx Zynq Ultrascale+ MPSoC", },
		{ }
};

int setup_known_board(const struct board **board, struct cs_devices_t *devices) {
	char const *board_name = getenv("CSAL_BOARD");
	if (board_name != NULL) {
		return setup_known_board_by_name(board_name, board, devices);
	}
	return setup_board(board, devices, known_boards);
}

int setup_known_board_by_name(const char *board_name, const struct board **board, struct cs_devices_t *devices) {
	return setup_named_board(board_name, board, devices, known_boards);
}
