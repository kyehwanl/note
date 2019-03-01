/*
 *			UNCLASSIFIED
 *		    FOR OFFICIAL USE ONLY
 * Copyright 2015-2016 Antara Teknik LLC.  All rights reserved.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

/*Unsigned integers*/
//typedef  u_int8_t  tUint8;
//typedef  u_int16_t tUint16;
typedef  u_int32_t tUint32;
typedef  u_int64_t tUint64;

typedef unsigned long long perf_cycles_t;

/* cpuid for intel e gnu compiler */
#if defined (__INTEL_COMPILER)
#define __CPUID(a, b)\
    __cpuid((a), (b))
#else /* #if defined (__INTEL_COMPILER) */
#define __CPUID(in,a,b,c,d)\
    asm volatile ("cpuid" \
                    : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
                    : "a" (in));
#endif /* #if defined (__INTEL_COMPILER) */

static void __inline__ taraCpuid(void)
{
#if defined (__INTEL_COMPILER)
    int CPUInfo[4] = {-1};
    __CPUID(CPUInfo, 1);
#else /* #if defined (__INTEL_COMPILER) */
    unsigned int a = 0x00, b = 0x00, c= 0x00, d = 0x00;
    __CPUID(0x00, a, b, c, d);
#endif /* #if defined (__INTEL_COMPILER) */
}
static __inline__ tUint64 taraRdtscp(void)
{
    volatile unsigned long a = 0, d = 0;
    tUint64 returnval = 0;

    taraCpuid();
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    //returnval = (((tUint64)d) << UPPER_HALF_OF_REGISTER);
    returnval = (((tUint64)d) << 32);
    returnval |= ((tUint64)a);

    return returnval;
}

perf_cycles_t taraCycleStamp(void)
{
    // works only with __x86_64__
    /*get time stamp twice, because we need to prime the timestamp counter*/
    taraRdtscp();
    return (perf_cycles_t)taraRdtscp();

}
//to enable the Perf compile with -D TARA_DO_PERF

#ifdef TARA_DO_PERF
        #define TARA_REPEAT_CNT 1 //1000000
        #define TARA_WARMUP_CNT 1 //TARA_REPEAT_CNT/4

        //performance vars
        perf_cycles_t StartingCycle, EndingCycle;
        double OperationCycles;

        //tUint32 measuredCpuSpeed = 0;

        int i=0;
        double OperationDuration;

        #define TARA_MEASURE_FTN_PERF(x)                                                \
            for (i=0; i< TARA_WARMUP_CNT; i++)	                                        \
                {x;}									\
        StartingCycle=taraCycleStamp();							\
	for (i = 0; i < TARA_REPEAT_CNT; i++)	                			\
	{										\
		{x;}									\
	}										\
        EndingCycle=taraCycleStamp();							\
	OperationCycles   =(double)(EndingCycle-StartingCycle)/TARA_REPEAT_CNT;         \
	OperationDuration = (double) OperationCycles / (double) measuredCpuSpeed;	\
        printf("Ave. Function time measured with repeatcount %d  :  %.2f usec\n\n",	\
			TARA_REPEAT_CNT, OperationDuration*1000000 );
#else
	#define TARA_MEASURE_FTN_PERF(x) {x;}
#endif

#include <stdio.h>
#include <unistd.h>



typedef long ChipRec_s32; ///< 32 bit signed integer
typedef long long ChipRec_s64; ///< 64 bit signed integer

typedef unsigned long ChipRec_u32; ///< 32 bit unsigned integer
typedef unsigned long long ChipRec_u64; ///< 64 bit unsigned integer


ChipRec_u64 taraGetCPUTick()
{

    // Use RDTSC To Read CPU Time Stamp Counter

    ChipRec_u64 u64Ret;
    __asm__ __volatile__ ("rdtsc" : "=A"(u64Ret):);
    return u64Ret;
}


tUint32 taraGetCPUSpeed()
{
    ChipRec_s64 startTick, endTick;
    static tUint32 cpuSpeed = 0;

    if(cpuSpeed ==0)
    {
        /*get the number of ticks in 1 second*/
        startTick = taraGetCPUTick();
        sleep(1);
        endTick = taraGetCPUTick();
        cpuSpeed = endTick - startTick;
    }

    // Return The Processors Speed In Hertz

    return cpuSpeed;
}


/*
void main(void)
{
  tUint32 measuredCpuSpeed = 0;
  measuredCpuSpeed =  taraGetCPUSpeed();
  //TARA_MEASURE_FTN_PERF (printf("."));
  TARA_MEASURE_FTN_PERF (sleep(1));
}
*/

//then to measure your function do: (without the //)

//TARA_MEASURE_FTN_PERF ((void) Some_function(its arguments) )
//
// How to compile: gcc -DTARA_DO_PERF -o perf_measurement perf_measurement.c
