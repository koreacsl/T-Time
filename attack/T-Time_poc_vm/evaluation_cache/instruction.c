#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <time.h>
#include <stdlib.h>
#include "pmparser.h"
#include "parse_pagemap.h"
#include "introspection.h"

extern void start();
extern void dummy1();
extern void target_function(int num);
extern void dummy2();

int main() {

	int num = 0;
	uint64_t ida[CodeLocationLength] = {
		0x000000005000, // target
		0x000000003000,	// start
	};



	printf("enter the num : ");
	scanf("%d",&num);
	if(num == 0)
	{
		for(int k=0;k<1200000;k++)
		{
			start(0);
		}
	}
	else
	{
		for(int k=0;k<1200000;k++)
			start(1);
	}

	printf("Done!\n");

	return 0;
}

