/*
 * app_to_trace.c
 *
 *  Created on: 15.01.2018
 *      Author: ujexq
 */


#include "platform.h"


#include <unistd.h>		/* for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

#include "primes.h"

void stop_app(void) {
	int c;
	fprintf(stderr, "[AppToTrace: press RETURN to continue or q/Q to quit]\r\n");
	fflush(stderr);
	c = getchar();
	if (c == 'q' || c == 'Q') {
		fprintf(stderr, "AppToTrace: exiting demo.\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {

    init_platform();


	printf("Demo application to trace\n");
//	stop_app();
	calculatePrimes();
	printf("End of Demo application\n");
    cleanup_platform();
    return EXIT_SUCCESS;
}

/* end of tracedemo.c */
