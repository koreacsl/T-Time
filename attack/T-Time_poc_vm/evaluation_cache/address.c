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
	dummy1();	
	uint64_t ida[CodeLocationLength] = {
		0x000000005000, // target
		0x000000003000,	// start
	};

	print_gpas(ida, CodeLocationLength);


	return 0;
}

