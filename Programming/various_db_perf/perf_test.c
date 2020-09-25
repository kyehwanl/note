/*
 * $Id: demo.c,v 1.4 2005/12/07 20:55:52 dplonka Exp $
 *
 * This is based on "demo.c" provided with MRT-2.2.2a.
 */

/*
 *  Usage: perf_test <file_name> [prefix]
 *
 */

#include "patricia.h"
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */

void func(prefix_t *prefix) {
    printf("node: %s/%d\n", prefix_toa(prefix), prefix->bitlen);
}

int
main(int argc, char *argv[])
{
    prefix_t *prefix;
    patricia_tree_t *tree;
    patricia_node_t *node;

    tree = New_Patricia(32);

    


    char *srcfile = NULL;
    if (argv[1] != NULL)
    {
        srcfile = argv[1];
    }

    FILE    *fp_srcfile;
    if ((fp_srcfile = fopen(srcfile, "r")) == NULL) {
        perror("FAIL: fopen()"); return -1;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, fp_srcfile)) != -1) 
    {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
#if 1
        prefix = ascii2prefix (AF_INET, line);
        node = patricia_lookup (tree, prefix);
        Deref_Prefix (prefix);
#endif
    }
    fclose(fp_srcfile);

#if defined (__TIME_MEASURE__)
#include "libtm_rdtsc.h"
    static unsigned int valCount=0;
    const unsigned int g_measureCount = 10;
    unsigned long long val_end_clock;

    tm_rdtsc_init(); // initialize time measure library
#endif /* __TIME_MEASURE__ */
    




    char* str;
    if (argv[2] != NULL)
    {
        str = argv[2];
    }
    else 
    {
        str = "2.232.71.192/26";
        //str = "2.241.0.0/17";
    }

    prefix = ascii2prefix (AF_INET, str);

#if defined (__TIME_MEASURE__)
    if(valCount == 0 && g_measureCount != 0)
        start_clock = rdtsc();
#endif /* __TIME_MEASURE__ */

    node = patricia_lookup (tree, prefix);


#if defined (__TIME_MEASURE__)
    valCount++;
    //if(valCount >= g_measureCount && g_measureCount != 0)
    {
        end_clock = rdtsc();
        printf("count reached %ld and terminate \n", g_measureCount);
        valCount =0;

        print_clock_time(end_clock, start_clock,      "Elapsed time");
    }
#endif /* __TIME_MEASURE__ */


    Deref_Prefix (prefix);





    make_and_lookup(tree, "2.241.0.0/17");
    try_search_best(tree, "2.241.0.0/16");
    try_search_best(tree, "2.241.0.0/18");
    try_search_exact(tree, "2.241.0.0/16");
    try_search_exact(tree, "2.241.0.0/18");

    make_and_lookup(tree, "10.42.42.0/24");
    make_and_lookup(tree, "10.42.69.0/24");
    make_and_lookup(tree, "10.0.0.0/8");

    make_and_lookup(tree, "2.27.0.0/16");
    try_search_best(tree, "2.27.0.0/8");
    try_search_best(tree, "2.27.0.0/24");
    try_search_exact(tree, "2.27.0.0/8");
    try_search_exact(tree, "2.27.0.0/24");


    Destroy_Patricia(tree, (void *)0);

    printf("Work Done \n");
    exit(0);
}






#if _COMMENT_

 in case of 500k nodes in patricia tree, 
 it took 4.6 us (worst case: to find the last one in order)
 it took 2.1 us (to find relatively first one)


 in case of 64k nodes in patricia tree, 
 it took average 3.3 us

#endif
