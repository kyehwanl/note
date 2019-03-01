#include "util_time_measure.h"

/* using perf_get_relative() --> clock_gettime() */
/* Relative time, since startup */
static struct timeval relative_time;
int perf_get_relative(struct timeval *tv)
{
  int ret;
  struct timespec tp;
  if (!(ret = clock_gettime (CLOCK_MONOTONIC, &tp)))
  {
    relative_time.tv_sec = tp.tv_sec;
    relative_time.tv_usec = tp.tv_nsec / 1000;
  }

  if (tv)
    *tv = relative_time;
  return ret;
}

time_t perf_clock(void)  // time_t : long int
{
  struct timeval tv;
  perf_get_relative(&tv);
  return tv.tv_sec;
}

/* using getrusage() */
void
thread_getrusage (RUSAGE_T *r)
{
  perf_get_relative (NULL);

  /* Return resource usage information on process indicated by RUSAGE_SELF(The calling process)
     and put it in r->cpu.  Returns 0 for success, -1 for failure.  */
  getrusage(RUSAGE_SELF, &(r->cpu));

  r->real = relative_time;
}

unsigned long
timeval_elapsed (struct timeval a, struct timeval b)
{
  return (((a.tv_sec - b.tv_sec) * TIMER_SECOND_MICRO)
	  + (a.tv_usec - b.tv_usec));
}

unsigned long
thread_consumed_time (RUSAGE_T *now, RUSAGE_T *start, unsigned long *cputime)
{
  /* This is 'user + sys' time.  */
  *cputime = timeval_elapsed (now->cpu.ru_utime, start->cpu.ru_utime) +
	     timeval_elapsed (now->cpu.ru_stime, start->cpu.ru_stime);
  return timeval_elapsed (now->real, start->real);
}




#if 0
#include <stdio.h>
/* test main */
int main(void)
{

#ifdef _TIME_MEASURE_ENABLED_
  unsigned long realtime, cputime;
  RUSAGE_T before, after;
  GETRUSAGE (&before);

  // TODO: dummy_test_routine();

  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  printf("SIGNING TASK: ran for %lums (cpu time %lums)\n", realtime/1000, cputime/1000);
#endif /* _TIME_MEASURE_ENABLED_ */


#ifdef _TIME_MEASURE_CLOCK_TIME_
  struct timeval start, end;
  perf_get_relative(&start);

  // TODO: dummy_test_routine();

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
#endif


/**
 * compile: NEED -lrt to link
 */

