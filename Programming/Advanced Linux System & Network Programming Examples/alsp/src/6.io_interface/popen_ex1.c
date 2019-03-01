/* vim: set ts=4 sw=4: */
/* Filename    : popen_ex1.c
 * Description : popen example for reading
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
	char	a_buf[65536];
	size_t	n_read;

	if ((fp_popen = popen("ls -l", "r")) == NULL) {
		fprintf(stderr, "FAIL: popen() (%d:%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (!feof(fp_popen)) {
		if ((n_read = fread(a_buf, sizeof(char), sizeof(a_buf), fp_popen)) == -1) { /* error */ }
		if (n_read == 0) break; /* EOF */
		printf("[%1$d byte] %2$.*1$s\n", (int)n_read, a_buf);
	}
	pclose(fp_popen);
	return (EXIT_SUCCESS);
}

