
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

static __inline__ unsigned long long rdtsc(void);
unsigned long long start_clock, end_clock;

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



#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif


int init()
{
  measuredCpuSpeed = taraGetCPUSpeed();
}

void print_clock_time(unsigned long long end_clock, unsigned long long start_clock, unsigned char* str)
{
  printf("cpu clock diff - %s(verify): %8.2f us \n", str, 1000000*(double)(end_clock - start_clock)/(double)measuredCpuSpeed);

  //if(1000000*(double)(end_clock - start_clock)/(double)measuredCpuSpeed < 100000) \
    fprintf(fp, "%8.2f\n", 1000000*(double)(end_clock - start_clock)/(double)measuredCpuSpeed); fflush(fp);
}



int main()
{
  init();

  start_clock = rdtsc();

  /*
   * HERE, locate a test program
   *    int c=0; for(c; c<1000; c++)
  */
  sleep(2);

  end_clock = rdtsc();
  print_clock_time(end_clock, start_clock, "testing");

  return 0;
}


/**
 * compile switch
 *  gcc -o time-measure-rdtsc time-measure-rdtsc.c -lrt
 *
 */
