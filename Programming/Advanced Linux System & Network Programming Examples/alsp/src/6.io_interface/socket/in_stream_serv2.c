/* vim: set ts=4 sw=4: */
/* Filename    : in_stream_serv2.c
 * Description : change server side socket option
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : prefork model of echo server
 * Todo        : make routine for signal SIGCHLD
 */
#include "io_macro.h"
#include "fork_socket.h"
#include <sys/wait.h>

#define MAX_POOL	3

int fd_listener;
void start_child(int fd, int idx);

int main(int argc, char *argv[])
{
	int		i, opt_sock; 
	short port; 
	socklen_t len_saddr;
	char	ipaddr[15+1];
	struct sockaddr_in	saddr_s;

	/* init variables */
	memset(ipaddr, 0x00, sizeof(ipaddr));
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc > 2) {
		printf(USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = (short) atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	pr_err("[TCP server] : Process Start");
	fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (fd_listener == -1) {
		pr_err("[TCP server] : Fail: socket()"); exit(EXIT_FAILURE);
	}
	opt_sock = 1;
	if (setsockopt(fd_listener, SOL_SOCKET, SO_REUSEADDR, &opt_sock, sizeof(opt_sock))) {
		exit(EXIT_FAILURE);
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
		switch(fork()) {
			case 0:		/* Child process */
				start_child(fd_listener, i);
				exit(EXIT_SUCCESS);
				break;
			case -1:	/* Error */
				pr_err("[TCP server] : Fail: fork()");
				break;
			default:	/* parent */
				printf("[TCP server] Making child process No.%d\n", i);
				break;
		}
	}
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
	int		cfd, ret_len; 
	socklen_t len_saddr;
	char	buf[40];	/* small buf */
	struct sockaddr_in	saddr_c;

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
			sleep(1); /* it's not special meaning */
		} /* packet recv loop */
	} /* main for loop */
}
