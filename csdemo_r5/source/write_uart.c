/*
 * write_uart.c
 *
 *  Created on: 17.01.2018
 *      Author: ujexq
 */
#include "xil_printf.h"
#include "xparameters.h"

/*
 * write_uchar8 -- write bytes to the serial port. Ignore fd, since
 *          stdout and stderr are the same. Since we have no filesystem,
 *          open will only return an error.
 */

int write_uchar8 (int32_t fd, unsigned char* buf, int32_t nbytes) {
#ifdef STDOUT_BASEADDRESS // UART BASE ADDRESS
  s32 i;
  unsigned char* LocalBuf = buf;

  (void)fd;
  for (i = 0; i < nbytes; i++) {
	if(LocalBuf != NULL) {
		LocalBuf += i;
	}
	if(LocalBuf != NULL) {
	    if (*LocalBuf == '\n') {
	      outbyte ('\r');
	    }
	    outbyte (*LocalBuf);
	}
	if(LocalBuf != NULL) {
		LocalBuf -= i;
	}
  }
  return (nbytes);
#endif
}
