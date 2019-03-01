#include <time.h>
#include <sys/resource.h>
#define TIMER_SECOND_MICRO 1000000L

struct rusage_t
{
  struct rusage cpu;
  struct timeval real;
};
#define RUSAGE_T        struct rusage_t
#define GETRUSAGE(X) thread_getrusage(X)


int perf_get_relative(struct timeval *);
time_t perf_clock(void);
void thread_getrusage (RUSAGE_T *);
unsigned long timeval_elapsed (struct timeval , struct timeval );
unsigned long thread_consumed_time (RUSAGE_T *, RUSAGE_T *, unsigned long *);
