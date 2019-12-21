/* vim: set ts=4 sw=4: */
/* Filename    : sect_memory.c
 * Description : various sections of memory
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int		num;		/* uninitiailized data : segment of .bss */
int		num_i = 1;	/* initialized data : segment of .data */

char	*p_str = "Hello, rodata";	/* "Hello, rodata" into .rodata.
									   'str' into .data and indicate to "Hello, ..." address.
									 */
char	arr_str[] = "Hello, array world";	/* str[] and "Hello, world" into .data */
const char	c_arr[] = "Hello, const array";

void print_hello();

int main()
{
	int		i;		/* Local auto variable : into stack memory */
	char	*p_buf;	/* Local auto variable : into stack memory */
	printf("Num = %d\n", num);	/* string "Num = %d\n" into .rodata */

	p_buf = (char *) malloc(1000);	/* allocated memory by malloc() : into heap memory */

	print_hello();

	return 0;
}

void print_hello()
{
	static int	i;	/* static variable into .bss */
	/* ... */
}
