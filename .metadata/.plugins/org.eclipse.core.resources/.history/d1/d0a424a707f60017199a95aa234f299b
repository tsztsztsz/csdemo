/*
 * STM Example
 *
 * Copyright (c) 2015-2016 ARM, Inc.  All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "stm.h"

#include "cambridge.h"
#include "primes.h"

struct STM *gSTM;

int main(int argc, char **argv)
{
	/* Initialize the STM */
	struct STM stm = { 0 };
	gSTM = (struct STM *) &stm;

	stmInit(
				gSTM,
				(struct stmAPB *) PLATFORM_STM_APB_ADDRESS,
				(struct stmAXI *) PLATFORM_STM_AXI_ADDRESS
			);

	stringExample();
	calculatePrimes();

	return 0;
}
