/*
 * STM Example
 *
 * Copyright (c) 2015-2016 ARM, Inc.  All rights reserved.
 *
 */

#include <stdio.h>
#include "stm.h"

void stringExample(void)
{
	int i = 0;

	printf("Sending string to STM...\n");
	for (; i < 65536; i++)
	{
		stmSendString(gSTM, 0, "Cambridge\nCambridge\nCambridge\nCambridge");
	}
	printf("Stop sending string to STM...\n");
}
