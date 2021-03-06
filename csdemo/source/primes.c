/*
 * Prime Number Generator Code
 * Modified for STM Example
 *
 * Copyright (c) 2011-2016 ARM Ltd.  All rights reserved.
 *
 */

//#include "stm.h"
#include <stdio.h>

#define TARGET_COUNT (512) //Number of primes to find

unsigned int target_count = TARGET_COUNT; // Number of primes to find, will equal TARGET_COUNT. Safe to reduce at debug time.
unsigned int prime_numbers[TARGET_COUNT] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, };
unsigned int prime_count = 12;
unsigned int next_number = 39;

static unsigned int getNextNumber()
{
	unsigned int number;
	number = next_number;
	next_number += 2;

	return number;
}

static void reportPrime(unsigned int number)
{
	prime_numbers[prime_count] = number;
	prime_count++;

	//STM32(gSTM->AXI, 1, G_D) = number;
	//STM32(gSTM->AXI, 1, G_DMTS) = prime_count;

	printf("prime number [%i]: %i\n",  prime_count, number);
	return;
}


void calculatePrimes(void)
{
	int number;
	int square;
	int remainder;
	int root = 1;
	int prime;
	int i;

//	STM32(gSTM->AXI, 1, G_FLAGTS) = 0;

	while(prime_count < target_count)
	{
		// Get the next number
		number = getNextNumber();

		square = root * root;

		while(number > square)
		{
			root++;
			square = root * root;
		}

		for(i = 1; i < prime_count; i++)
		{
			prime = prime_numbers[i];

			if (prime > root)
			{
				reportPrime(number);
				break;
			}

			remainder = number % prime;

			if (remainder == 0)
			{
				// not a prime, so discard number
				break;
			}
		}
	}
	return;
}
