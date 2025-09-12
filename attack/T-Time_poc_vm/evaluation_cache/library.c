#include "library.h"

// IMUL
#define IMUL "imul %%r12, %%r13\n"

// 10 times
#define REPEAT_10  \
    IMUL IMUL IMUL IMUL IMUL \
    IMUL IMUL IMUL IMUL IMUL

// 50 times
#define REPEAT_50 \
	REPEAT_10 REPEAT_10 REPEAT_10 REPEAT_10 REPEAT_10 \

// 100 times
#define REPEAT_100 \
    REPEAT_10 REPEAT_10 REPEAT_10 REPEAT_10 REPEAT_10 \
    REPEAT_10 REPEAT_10 REPEAT_10 REPEAT_10 REPEAT_10

// Define NOP
#define NOP "nop\n"

// Repeat 10 times
#define REPEAT_NOP_10 \
    NOP NOP NOP NOP NOP \
    NOP NOP NOP NOP NOP

// Repeat 50 times (5 * 10)
#define REPEAT_NOP_50 \
    REPEAT_NOP_10 REPEAT_NOP_10 REPEAT_NOP_10 REPEAT_NOP_10 REPEAT_NOP_10

// Repeat 100 times (2 * 50)
#define REPEAT_NOP_100 \
    REPEAT_NOP_50 REPEAT_NOP_50

void start(int num) __attribute__((aligned(4096)));
void dummy1() __attribute__((aligned(4096)));
void target_function(int num) __attribute__((aligned(4096)));
void dummy2() __attribute__((aligned(4096)));

void start(int num)
{
    asm volatile(
        "mov $0x2, %%r12\n\t"
	"mov $0x2, %%r13\n\t"
        :
        :
        : "r12","r13"
    );	
	target_function(num);
}

void dummy1()
{
    return;
}

void target_function(int num) {
    if(num == 0)
    {	    
        asm volatile (
			
	REPEAT_10
        "pop %%r12\n\t"
        "pop %%r13\n\t"
	"pop %%rbp\n\t"
	"ret\n\t"
	REPEAT_NOP_100
	REPEAT_NOP_50
	REPEAT_NOP_10
	REPEAT_NOP_10
	REPEAT_NOP_10
	NOP
	NOP
	NOP
            ::: "r12","r13"
        );
    }
    else
    {
        asm volatile (
	    REPEAT_10
            ::: "r12","r13"			
        );	    
    }
}

void dummy2()
{
    return;
}

