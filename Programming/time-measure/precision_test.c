#include <stdio.h>
#include <time.h>

#define SEC2NANOSEC 1000000000

int main( int argc, const char* argv[] )
{
    // //////////////// Min resolution test //////////////////////
    struct timespec resStart, resEnd, ts;
    ts.tv_sec  = 0; // s
    ts.tv_nsec = 1; // ns
    int iters = 100;
    double resTime,sum = 0;
    int i;
    for (i = 0; i<iters; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &resStart);      // start timer
        // clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, &ts);
        clock_gettime(CLOCK_MONOTONIC, &resEnd);        // end timer
        resTime = ((double)resEnd.tv_sec*SEC2NANOSEC + (double)resEnd.tv_nsec - (double)resStart.tv_sec*SEC2NANOSEC + (double)resStart.tv_nsec);
        sum = sum + resTime;
        printf("resTime = %f\n",resTime);
    }
    printf("Average = %f\n",sum/(double)iters);
}


/*
 * compile: gcc soExample.c -o runSOExample -lrt
 */
