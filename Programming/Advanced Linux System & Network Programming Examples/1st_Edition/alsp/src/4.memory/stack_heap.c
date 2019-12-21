/* vim: set ts=4 sw=4: */
/* Filename    : stack_heap.c
 * Description : exhaust stack or heap
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define SZ_BUFFER 1024*1024		/* 1MB */

void pr_usages(char *arg);
int exhaust_stack(int count);
int exhaust_heap(int count);

int main(int argc, char *argv[])
{
	int		sz_mem;

	if (argc != 3) {
		pr_usages(argv[0]);
		exit(EXIT_SUCCESS);
	}

	sz_mem = atoi(argv[2]);
	switch (argv[1][0]) {
		case 's':
			printf("Will exhaust stack memory to %d MB, call func(%p)\n", sz_mem, exhaust_stack);
			exhaust_stack(sz_mem);
			break;
		case 'h':
			printf("Will exhaust heap memory to %d MB, call func(%p)\n", sz_mem, exhaust_heap);
			exhaust_heap(sz_mem);
			break;
		default:
			printf("*** UNKNOWN OPTIONS ***\n");
			pr_usages(argv[0]);
			exit(EXIT_SUCCESS);
	}

	return 0;
}

void pr_usages(char *arg)
{
	printf("Usage : %s <s|h> <memory size, MB>\n", arg);
	printf(" -options\n");
	printf("  s       will exhaust stack memory.\n");
	printf("  h       will exhaust heap memory.\n");
	printf("  size    memory size(MB) to exhaust.\n");
	printf(" - ex: %s s 10       will exhaust stack memory to 10 MB\n", arg);
	printf("       (first of all, check stack size(KB) 'ulimit -s'\n");
}

int exhaust_stack(int count)
{
	char buffer[SZ_BUFFER]; /* 1 MB stack */

	if (count <= 0) {
		printf(">> reach break position, stop recursive function!!\n");
		return 0;
	}
	sprintf(buffer, ">> exhaust 1MB stack(addr:%p), will more %d MB...\n", buffer, count-1);
	printf(buffer);
	exhaust_stack(count-1);
	return 0;
}

int exhaust_heap(int count)
{
	char *p_buf;

	if (count <= 0) {
		printf(">> reach break position, stop recursive function!!\n");
		return 0;
	}
	p_buf = (char *)malloc(SZ_BUFFER);
	if (p_buf == NULL) {
		printf(">> ERR: malloc(), count(%d), err(%s)\n", count, strerror(errno));
		return -1;
	}
	sprintf(p_buf, ">> exhaust 1MB heap(addr:%p), will more %d MB...\n", p_buf, count-1);
	printf(p_buf);
	exhaust_heap(count-1);
	free(p_buf);
	return 0;
}
