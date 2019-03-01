/* vim: set ts=4 sw=4: */
/* Filename    : sig_nocldstop.c
 * Description : Test NOCLDSTOP
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void chk_chld(int signum);

int main()
{
	int		ret;
	struct sigaction sa_chld;

	memset(&sa_chld, 0, sizeof(struct sigaction));
	sa_chld.sa_handler = chk_chld;
	sigfillset(&sa_chld.sa_mask);	/* blocking signal. */
	sa_chld.sa_flags = SA_NOCLDSTOP;

	sigaction(SIGCHLD, &sa_chld, NULL);
	printf("[MAIN] SIGNAL Handler installed\n");

	printf("[MAIN] pid is %d\n", getpid());
	switch((ret = fork())) {
		case 0:
			pause();
			exit(EXIT_SUCCESS);
		case -1:
			break;
		default:
			printf("- Child pid = %d\n", ret);
			break;
	}
		
	while (1) {
		pause();
		printf("[MAIN] Recv SIGNAL...\n");
	}
	return EXIT_SUCCESS;
}

/* Name : signal handler func about SIGCHLD
 * Desc : 
 * Argv : 
 * Ret  : None
 */
void chk_chld(int signum)
{
	pid_t	pid_child;
	int		status;

	printf("[SIGNAL] RECV SIGCHLD signal\n");
	if ((pid_child = waitpid(-1, &status, WNOHANG)) > 0) {
		printf("\t- Exit child PID(%d)\n", pid_child);

	}
}
