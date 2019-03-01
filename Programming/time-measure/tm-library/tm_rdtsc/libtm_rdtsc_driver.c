#include "libtm_rdtsc.h"



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


/**
 * compile: NEED -lrt to link
 * gcc -o libtm_rdtsc_driver libtm_rdtsc_driver.c -L./.libs/ -ltm_rdtsc -lrt -Wl,-rpath -Wl,/users/kyehwanl/Programming/time-measure/tm-library/.libs/
 */

