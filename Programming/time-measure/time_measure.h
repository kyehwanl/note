#ifndef __TIME_MEASURE_H__
#define __TIME_MEASURE_H__

#define UPDATE_COUNT 500000

#include <sys/time.h>

struct timeval start, end;
struct timeval start_v, end_v;
static int orderReceipt, orderNoReceipt;
static unsigned long ul_sign_count;
static unsigned long ul_verify_count;
static int type;

#define PRINT_TIME(start, end, fmt, order)  \
do {					    \
    long seconds, useconds, mtime;	    \
    seconds  = end.tv_sec  - start.tv_sec;  \
    useconds = end.tv_usec - start.tv_usec; \
    if(useconds < 0){			    \
	seconds--;	  		    \
	useconds = end.tv_usec+1000000 - start.tv_usec;		    \
    }								    \
    mtime = useconds / 1000.0;					    \
    useconds %= 1000;						    \
    printf("--- Elapsed time(%s #%ld): %ld sec %ld ms %ld us\n", fmt, order, seconds, mtime, useconds); \
    printf("  - start_sec: %ld end_sec: %ld\n",start.tv_sec,end.tv_sec);    \
    printf("  - start_usec: %ld end_usec: %ld\n",start.tv_usec,end.tv_usec);\
} while(0)

#define TIME_MEASURE_START(type)            \
do {                                        \
  if(type==0)  {                            \
    if (ul_sign_count == 0)                 \
      gettimeofday(&start, NULL);           \
    ul_sign_count++;                        \
  }                                         \
  else {                                    \
    if (ul_verify_count == 0)               \
      gettimeofday(&start_v, NULL);         \
    ul_verify_count++;                      \
  }                                         \
} while(0)


#define TIME_MEASURE_END(type)              \
do {                                        \
  if(type==0) {                             \
    if (ul_sign_count >= g_update) {        \
      gettimeofday(&end, NULL);             \
      PRINT_TIME(start, end, " INTERVAL: sign_with_key ", ul_sign_count);   \
    }                                       \
  }                                         \
  else {                                    \
    if (ul_verify_count >= g_update) {      \
      gettimeofday(&end_v, NULL);           \
      PRINT_TIME(start_v, end_v, " INTERVAL: verify_with_key ", ul_verify_count); \
    }                                       \
  }                                         \
} while(0)

#endif /* __TIME_MEASURE_H__ */

