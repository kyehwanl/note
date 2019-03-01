/* vim: set ts=4 sw=4: */
/* Filename    : execl.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <unistd.h>

int main()
{
	if (execl("/bin/ls", "ls", "-al", NULL) == -1) {
		perror("execl");
	}
	printf("+ after execl\n");
	return 0;
}
