/* vim: set ts=4 sw=4: */
/* Filename    : sig_basic.c
 * Description : simple signal model
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : sigaction & signal mask
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void chk_sigusr(int signum);

int main()
{
	struct sigaction sa_usr1;
	struct sigaction sa_usr2;

	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = chk_sigusr;
	sigfillset(&sa_usr1.sa_mask);	/* blocking signal. So it can't be allowed interrupted by other signal */
#ifdef WITH_SA_NOMASK
	sa_usr1.sa_flags = SA_NODEFER;
#endif

	memset(&sa_usr2, 0, sizeof(struct sigaction));
	sa_usr2.sa_handler = chk_sigusr;
	sigemptyset(&sa_usr2.sa_mask);	/* empty signal mask, so other process can interrupt when is running this handler */
#ifdef WITH_SA_NOMASK
	sa_usr2.sa_flags = SA_NODEFER;
#endif

	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	printf("[MAIN] SIGNAL Handler installed\n");
	/* USR1 의 경우, 시그널 매스크가 채워져 있으므로 USR1 시그널 처리기가 실행되는 동안 
	   다른 시그널은 대기하게 된다. 따라서 USR1 시그널 처리가 끝난뒤에 다른 시그널이 실행된다.
	   하지만, USR2 는 시그널 매스크가 비어있으므로 모든 시그널이 통과하게 되고,
	   처리기가 실행되는 도중에 다른 시그널이 도착하면 잠시 중지하고 도착한 시그널 핸들러가 실행된다.
	 */
	printf("[MAIN] pid is %d\n", getpid());
	for(;;) {
		pause();
		printf("[MAIN] Recv SIGNAL...\n");
	}
	return EXIT_SUCCESS;
}

/* Name : signal handler func about SIGUSR1 & SIGUSR2
 * Desc : sleep 10 sec & print message every second.
 * Argv : 
 * Ret  : None
 */
void chk_sigusr(int signum)
{
	int	i;
	for (i=0; i<10; i++) {
		printf("\tSignal(%s):%d sec.\n", signum == SIGUSR1 ? "USR1":"USR2", i);
		sleep(1);
	}
}
