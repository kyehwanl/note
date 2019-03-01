#ifndef _TM_RDTSC_H_
#define _TM_RDTSC_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

typedef  u_int32_t tUint32;
typedef  u_int64_t tUint64;

typedef long ChipRec_s32; ///< 32 bit signed integer
typedef long long ChipRec_s64; ///< 64 bit signed integer

typedef unsigned long ChipRec_u32; ///< 32 bit unsigned integer
typedef unsigned long long ChipRec_u64; ///< 64 bit unsigned integer

tUint32 measuredCpuSpeed;



/**
 * Function Declarations
 */
unsigned long long rdtsc(void);
unsigned long long start_clock, end_clock, clk_t0, clk_t1, clk_t2, clk_t3, clk_t4, clk_t5;
void tm_rdtsc_init(void);
void print_clock_time(unsigned long long, unsigned long long, unsigned char*);
#endif /* _TM_RDTSC_H_ */
