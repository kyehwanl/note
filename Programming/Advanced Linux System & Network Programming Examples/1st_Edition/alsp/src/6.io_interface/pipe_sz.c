/* vim: set ts=4 sw=4: */
/* Filename    : pipe_sz.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <limits.h>	/* Get PIPE_BUF size */
int main()
{
	printf("PIPE Size = %d\n", PIPE_BUF); /* has garantee atomic operation at writing */
	return 0;
}

