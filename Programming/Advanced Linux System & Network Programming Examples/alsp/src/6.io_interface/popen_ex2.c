/* vim: set ts=4 sw=4: */
/* Filename    : popen_ex2.c
 * Description : popen example for writing
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main()
{
	FILE *fp_popen;
	char	a_buf[0xff];
	int		i, n_write;

	if ((fp_popen = popen("sort", "w")) == NULL) {
		fprintf(stderr, "FAIL: popen() (%d:%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("* waiting for your input : \n");

	for(i=0; i<5; i++) {
		if (fgets(a_buf, sizeof(a_buf), stdin) == NULL) { /* error */ }
		if ((n_write = fwrite(a_buf, sizeof(char), strlen(a_buf), fp_popen)) == -1) { /* error */ }
		if (n_write == 0) break; /* broken pipe */
	}
	printf("* Sorting data ->\n");
	pclose(fp_popen);
	return (EXIT_SUCCESS);
}

