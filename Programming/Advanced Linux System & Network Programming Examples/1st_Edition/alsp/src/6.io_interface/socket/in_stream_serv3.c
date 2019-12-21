/* vim: set ts=4 sw=4: */
/* Filename    : in_stream_serv3.c
 * Description : prefork child and if killed child, refork one.
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : prefork model of echo server
 */
#include "io_macro.h"
#include "fork_socket.h"
#include <sys/wait.h>

#define MAX_POOL	3
#define SERVTYPE_SERVER		0
#define SERVTYPE_CLIENT		1

int fd_listener;
int	proc_tab[MAX_POOL];

void start_child(int fd, int idx);
void set_signalset();
void kill_child(int signum);
void restart_child(int signum);

int main(int argc, char *argv[])
{
	char	ipaddr[15+1];
	socklen_t len_saddr;
	struct sockaddr_in	saddr_s;
	short	port;
	/* logical var */
	int		i;
	pid_t	pid;

	/* init variables */
	memset(ipaddr, 0x00, sizeof(ipaddr));
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc > 2) {
		printf(USAGE, argv[0]); exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = (short) atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	pr_out("[TCP server] : Process Start");
	fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (fd_listener == -1) {
		pr_err("[TCP server] : Fail: socket()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	saddr_s.sin_port = htons(port);

	if (bind(fd_listener, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("[TCP server] : Fail: bind()"); exit(EXIT_FAILURE);
	}

	if (port == 0) {
		len_saddr = sizeof(saddr_s);
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
	}
	printf("[TCP server] : Port : #%d\n", ntohs(saddr_s.sin_port));

	listen(fd_listener, LISTEN_BACKLOG);
	for(i=0; i<MAX_POOL; i++) {
		
		/* fork immediately */
		switch(pid = fork()) {
			case 0:		/* Child process */
				start_child(fd_listener, i);
				exit(EXIT_FAILURE);
				break;
			case -1:	/* Error */
				pr_err("[TCP server] : Fail: fork()");
				proc_tab[i] = -1;
				break;
			default:	/* parent */
				printf("[TCP server] Making child process No.%d : pid=%d\n", i, pid);
				proc_tab[i] = pid;
				break;
		}
	}

	set_signalset(SERVTYPE_SERVER);
	for(;;) pause();

	return 0;
}

/* Name : start_child
 * Desc : echo about socket stream.
 * Argv : int fd (socket descriptor)
 * Ret  : None
 */
void start_child(int sfd, int idx)
{
	int		cfd;
	int		ret_len; 
	unsigned int  len_saddr;
	char	buf[40];	/* small buf */
	struct sockaddr_in	saddr_c;

	/* signal default */
	set_signalset(SERVTYPE_CLIENT);

	/* echo server */
	len_saddr = sizeof(struct sockaddr);

	for(;;) {
		cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr);
		if (cfd == -1) {
			pr_err("[Child] : Fail: accept()");
			close(cfd);
			continue;
		}
		printf("[Child:%d] Accept about socket(%d)\n", idx, cfd);

		for(;;) {
			sleep(10);
			ret_len = recv(cfd, buf, sizeof(buf), 0);
			if (ret_len == -1) {
				if (errno == EINTR) continue;
				fprintf(stderr, "[Child:%d] Fail: recv(): %s\n", idx, strerror(errno));
				break;
			}
			if (ret_len == 0) {
				fprintf(stderr, "[Child:%d] Session closed\n", idx);
				close(cfd);
				break;;
			}

			printf("[Child:%d] RECV(%.*s)\n", idx, ret_len, buf);
			if (send(cfd, buf, ret_len, 0) == -1) {
				fprintf(stderr, "[Child:%d] Fail: send() to socket(%d)\n", idx, cfd);
				close(cfd);
			}
			sleep(1);
		} /* packet recv loop */
	} /* main for loop */
}

/* Name : set_signalset
 * Desc : install signal handler
 * Argv : None
 * Ret  : None
 */
void
set_signalset(int type)
{
	struct sigaction sigact_term;
	struct sigaction sigact_chld;
	struct sigaction sigact_dfl;

	if (type == SERVTYPE_SERVER) {
		sigact_term.sa_handler = kill_child;
		sigemptyset(&sigact_term.sa_mask);
		sigact_term.sa_flags = SA_NOCLDSTOP;

		sigaction(SIGTERM, &sigact_term, NULL);
		sigaction(SIGINT, &sigact_term, NULL);

		/* 짧은 시간동안에 두개의 child가 죽은 경우에는 잡아낼 수 없다. 
		 * 이 문제를 해결하려면? */
		sigact_chld.sa_handler = restart_child;
		sigfillset(&sigact_term.sa_mask);
		sigact_term.sa_flags = SA_NOCLDSTOP;

		sigaction(SIGCHLD, &sigact_chld, NULL);
	} else {
		sigact_dfl.sa_handler = SIG_DFL;
		sigfillset(&sigact_dfl.sa_mask);
		sigact_dfl.sa_flags = SA_RESTART;

		sigaction(SIGTERM, &sigact_dfl, NULL);
		sigaction(SIGINT, &sigact_dfl, NULL);
	}
}

/* Name : kill_child
 * Desc : signal handler:Kill all child process on proc_tab[]
 * Argv : int signum
 * Ret  : None
 */
void
kill_child(int signum)
{
	int i;

	for(i=0; i<MAX_POOL; i++) {
		if (proc_tab[i] > 0) {
			kill((pid_t)proc_tab[i], SIGTERM);
			printf("[TCP server:%d] killed child process No.%d (pid %d)\n", getpid(), i, proc_tab[i]);
		}
	}
	close(fd_listener);
	exit(EXIT_FAILURE);
}

/* Name : restart_child
 * Desc : signal handler about SIGCHLD signal
 * Argv : int signum
 * Ret  : None
 */
void
restart_child(int signum)
{
	int i, status;
	pid_t	pid;

	if ((pid = waitpid(-1, &status, WNOHANG)) <= 0) {
		pr_err("[TCP server:%d] Fail: waitpid()", getpid());
		return;
	}

	for(i=0; i<MAX_POOL; i++) {
		if (proc_tab[i] == pid) {
			printf("[TCP server:%d] Child(%d) Status : %d\n", getpid(), proc_tab[i], status);
			/* fork immediately */
			switch(pid = fork()) {
				case 0:		/* Child process */
					start_child(fd_listener, i);
					exit(EXIT_SUCCESS);
					break;
				case -1:	/* Error */
					pr_err("[TCP server] : Fail: fork()");
					proc_tab[i] = -1;
					break;
				default:	/* parent */
					pr_out("[TCP server] Making child process No.%d : pid=%d", i, pid);
					proc_tab[i] = pid;
					break;
			}
			break;
		}
	} /* main loop */
	if (i == MAX_POOL) {
		pr_out("[TCP server] Can't find specified child process killed");
	}
}

