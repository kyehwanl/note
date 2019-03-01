
/*
 * This is driver file for utilizing tm-library.
 * Below is the two way usage of using this library.
 *
 */


#include <stdio.h>
#define _TIME_MEASURE_ENABLED_
#define _TIME_MEASURE_CLOCK_TIME_
//#define _TIME_MEASURE_SEC_
#include "util_time_measure.h"
unsigned long realtime, cputime;
RUSAGE_T before, after;

/* test main */
int main(void)
{

  GETRUSAGE (&before);

  // TODO: dummy_test_routine();
  usleep(21);

  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  printf(" TASK: ran for %lu us (cpu time %lu us)\n", realtime, cputime);


#ifdef _TIME_MEASURE_CLOCK_TIME_
  struct timeval start, end;
  perf_get_relative(&start);

  // TODO: dummy_test_routine();
  sleep(2);

  perf_get_relative(&end);
  printf(" *** elapsed time(us):  %lu\n", timeval_elapsed (end, start));
#endif /* _TIME_MEASURE_CLOCK_TIME_*/

#ifdef _TIME_MEASURE_SEC_
  time_t start_time = perf_clock ();
  // TODO: dummy_test_routine();
  time_t end_time = perf_clock ();
  printf("+++ TEST1 [perf_test_clock] --  %lu (0x%08x)\n", start_time, start_time);
  printf("+++ TEST1 [perf_test_clock] --  %lu (0x%08x)\n", end_time, end_time);
  printf("+++ TEST1 result : %lu\n", end_time - start_time);
#endif /* _TIME_MEASURE_SEC_*/



  return 0;
}


/**
 * compile: NEED -lrt to link
 * gcc -o util_main util_main.c -L./.libs/ -ltm_util -lrt -Wl,-rpath -Wl,/users/kyehwanl/Programming/time-measure/tm-library/.libs/
 */

