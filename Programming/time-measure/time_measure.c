#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>

#define TIMER_SECOND_MICRO 1000000L

static unsigned long
timeval_elapsed (struct timeval a, struct timeval b)
{
  return (((a.tv_sec - b.tv_sec) * TIMER_SECOND_MICRO)
	  + (a.tv_usec - b.tv_usec));
}

int main() {
  struct rusage usage;
  struct timeval start, end;
  struct timeval s_start, s_end;
  int i, j, k = 0;

  struct timespec ts;
  ts.tv_sec = 3;
  ts.tv_nsec = 100000000;


  getrusage(RUSAGE_SELF, &usage);
  start = usage.ru_utime;
  s_start = usage.ru_stime;
#if 0
  for (i = 0; i < 10000; i++) {
    /* Double loop for more interesting results. */
    for (j = 0; j < 10000; j++) {
      k += 20;
    }
  }
  nanosleep(&ts, NULL);
#endif
  getrusage(RUSAGE_SELF, &usage);
  end = usage.ru_utime;
  s_end = usage.ru_stime;

  printf("Started at: %lus %lums (%lus %lums)\n", start.tv_sec, start.tv_usec/1000, s_start.tv_sec, s_start.tv_usec/1000);
  printf("Ended at: %lus %lums (%lus %lums)\n", end.tv_sec, end.tv_usec/1000, s_end.tv_sec, s_end.tv_usec/1000);

  unsigned long cputime;
  cputime = timeval_elapsed (end, start) + timeval_elapsed (s_end, s_start);
  printf ("\nTASK(single): ran for cpu time %lums\n", cputime/1000);

  printf(" nanosleep performed for %lu ns\n", ts.tv_nsec);
  unsigned long ulTest1 = 4930000000;
  unsigned long long ulTest2 = 4920000000;
  printf(" test print %lu \n", ulTest1 );
  printf(" test print %llu \n", ulTest1 );
  printf(" test print %lu \n", ulTest2 );
  printf(" test print %llu \n", ulTest2 );
  printf(" test print %lld %lld\n", ulTest2, ulTest2 );
  return 0;
}
