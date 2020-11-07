/*
 * $Id: demo.c,v 1.4 2005/12/07 20:55:52 dplonka Exp $
 *
 * This is based on "demo.c" provided with MRT-2.2.2a.
 *
 *  Usaage: arr_test [value]
 */

#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */

int binarySearch(int arr[], int l, int r, int x) 
{ 
    if (r >= l) { 
        int mid = l + (r - l) / 2; 
  
        // If the element is present at the middle 
        // itself 
        if (arr[mid] == x) 
            return mid; 
  
        // If element is smaller than mid, then 
        // it can only be present in left subarray 
        if (arr[mid] > x) 
            return binarySearch(arr, l, mid - 1, x); 
  
        // Else the element can only be present 
        // in right subarray 
        return binarySearch(arr, mid + 1, r, x); 
    } 
  
    // We reach here when element is not 
    // present in array 
    return -1; 
} 

#if 0 // BINARY search main
int main(void) 
{ 
    int arr[] = { 2, 3, 4, 10, 40 }; 
    int n = sizeof(arr) / sizeof(arr[0]); 
    int x = 10; 
    int result = binarySearch(arr, 0, n - 1, x); 
    (result == -1) ? printf("Element is not present in array\n") 
                   : printf("Element is present at index %d\n", 
                            result); 
    return 0; 
} 
#endif
  

#if 1
int
main(int argc, char *argv[])
{
   


    char *find = NULL;
    if (argv[1] != NULL)
    {
        find = argv[1];
    }
    else 
    {
        find = "100";
    }
    int find_val = atoi(find);


#if defined (__TIME_MEASURE__)
#include "libtm_rdtsc.h"
    static unsigned int valCount=0;
    const unsigned int g_measureCount = 10;
    unsigned long long val_end_clock;

    tm_rdtsc_init(); // initialize time measure library
#endif /* __TIME_MEASURE__ */
    

//#define MAX_CNT 65536
#define MAX_CNT 100000
    unsigned int as_list[MAX_CNT];
    int result;
    int i=0;
    //as_list = (unsigned short*) malloc(size(unsigned short)* 65536);
    //
    for (i=0; i< MAX_CNT; i++)
    {
        as_list[i] = i+1;
    }
    int found = 0;

    int n = sizeof(as_list) / sizeof(as_list[0]); 
    int x = find_val;


#if defined (__TIME_MEASURE__)
    if(valCount == 0 && g_measureCount != 0)
        start_clock = rdtsc();
#endif /* __TIME_MEASURE__ */

#define  BINARY_SEARCH
//#define  LINEAR_SEARCH

#ifdef LINEAR_SEARCH
    for (i=0; i < MAX_CNT; i++)
    {
        if (as_list[i] == find_val)
        {
            found = 1;
            break;
        }

    }
#endif

#ifdef BINARY_SEARCH

    result = binarySearch(as_list, 0, n - 1, x); 

#endif


#if defined (__TIME_MEASURE__)
    //valCount++;
    //if(valCount >= g_measureCount && g_measureCount != 0)
    {
        end_clock = rdtsc();
        printf("count reached %ld and terminate \n", g_measureCount);
        //valCount =0;

        print_clock_time(end_clock, start_clock,      "Elapsed time");
    }
#endif /* __TIME_MEASURE__ */

    result = found;
    (result == -1) ? printf("Element is not present in array\n") 
                   : printf("Element is present at index %d\n", result); 
    if (result != -1)
        found = 1;

    if(found)
        printf(" value found : %d\n", find_val);


    printf("Work Done \n");
    exit(0);
}
#endif


#if _COMMENT_

 it defers range from 0.07us to 165us (Closet to farthest) 

But BinarySearch in a sorted array takes 0.3 us at average

#endif



