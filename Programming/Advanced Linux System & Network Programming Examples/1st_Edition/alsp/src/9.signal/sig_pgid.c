/* vim: set ts=4 sw=4: */
/* Filename    : sig_pgid.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#ifdef _XOPEN_SOURCE
# if (_XOPEN_SOURCE - 0) < 500
# undef _XOPEN_SOURCE
# endif
#endif
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void chk_sigusr(int signum);

int main()
{
	int		i;
	struct sigaction sa_usr1, sa_usr2;

	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = chk_sigusr;
	sigfillset(&sa_usr1.sa_mask);	/* fill blocking signal mask. */

	memset(&sa_usr2, 0, sizeof(struct sigaction));
	sa_usr2.sa_handler = chk_sigusr;
	sigfillset(&sa_usr2.sa_mask);

	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	printf("++ PID(%d) PGID(%d) SID(%d)\n", getpid(), getpgid(0), getsid(0));
	for(i=0; i<3; i++) {
		if (fork() == 0) break;
	}
	for(;;) {
		pause();
		printf("[MAIN] Recv SIGNAL...\n");
	}
	return EXIT_SUCCESS;
}

/* Name : signal handler func about SIGUSR1 & SIGUSR2
 * Desc : 
 * Argv : 
 * Ret  : None
 */
void chk_sigusr(int signum)
{
	switch(signum) {
		case SIGUSR1:
			printf("-- PID(%d) PGID(%d) SID(%d)\n", getpid(), getpgid(0), getsid(0));
			break;
		case SIGUSR2:
			if (getpid() != getpgid(0)) {
				setpgid(0, 0);
			} else {
				setpgid(0, getppid());
			}
			printf("-- PID(%d) to PGID(%d)\n", getpid(), getpgid(0));
			break;
	}
}

