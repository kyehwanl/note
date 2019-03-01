#include "libtm_rdtsc.h"


ChipRec_u64 getCPUTick()
{

    // Use RDTSC To Read CPU Time Stamp Counter
    ChipRec_u64 u64Ret;
    __asm__ __volatile__ ("rdtsc" : "=A"(u64Ret):);
    return u64Ret;
}


tUint32 getCPUSpeed()
{
    ChipRec_s64 startTick, endTick;
    static tUint32 cpuSpeed = 0;

    if(cpuSpeed ==0)
    {
        /*get the number of ticks in 1 second*/
        startTick = getCPUTick();
        sleep(1);
        endTick = getCPUTick();
        cpuSpeed = endTick - startTick;
    }

    // Return The Processors Speed In Hertz
    return cpuSpeed;
}



#if defined(__i386__)
//static __inline__ unsigned long long rdtsc(void)
unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)
//static __inline__ unsigned long long rdtsc(void)
unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif


void tm_rdtsc_init(void)
{
  measuredCpuSpeed = getCPUSpeed();
}

#include <locale.h>
void print_clock_time(unsigned long long end_clock, unsigned long long start_clock, unsigned char* str)
{
  setlocale(LC_ALL, "");
  printf(" -- %-50s: %'8.2f us \t %'8.2f ms\n",
      str,
      1000000*(double)(end_clock - start_clock)/(double)measuredCpuSpeed,
      1000*(double)(end_clock - start_clock)/(double)measuredCpuSpeed);

}

#ifdef __MAIN__
int main()
{
  tm_rdtsc_init();

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
#endif


/**
 * compile: NEED -lrt to link
 */

