

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*Unsigned integers*/
typedef  u_int32_t tUint32;
typedef  u_int64_t tUint64;

/*Signed integers*/
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

static void __inline__ taraCpuid(void);
static __inline__ tUint64 taraRdtscp(void);
perf_cycles_t taraCycleStamp(void);



#ifdef TARA_DO_PERF
        #define TARA_REPEAT_CNT 1 //1000000
        #define TARA_WARMUP_CNT 1 //TARA_REPEAT_CNT/4

        //performance vars
        perf_cycles_t StartingCycle, EndingCycle;
        double OperationCycles;

        tUint32 measuredCpuSpeed = 0;

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

typedef long ChipRec_s32; ///< 32 bit signed integer
typedef long long ChipRec_s64; ///< 64 bit signed integer

typedef unsigned long ChipRec_u32; ///< 32 bit unsigned integer
typedef unsigned long long ChipRec_u64; ///< 64 bit unsigned integer


ChipRec_u64 taraGetCPUTick();
tUint32 taraGetCPUSpeed();
