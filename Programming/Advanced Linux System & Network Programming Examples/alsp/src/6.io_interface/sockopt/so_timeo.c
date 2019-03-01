/* vim: set ts=4 sw=4: */
/* Filename    : so_timeo.c
 * Description : apply SO_RCVTIMEO option for tcp (available on udp too)
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : prefork model of TCP echo server
 * Todo        : make routine for signal SIGCHLD
 */
#include "io_macro.h"
#include "fork_socket.h"

#define MAX_POOL	3

int fd_listener;
void start_child(int fd, int idx);

int main(int argc, char *argv[])
{
	int		i;
	socklen_t len_saddr;
	short	port;
	char	ipaddr[15+1];
	struct sockaddr_in	saddr_s;
	pid_t	pid;

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

	fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (fd_listener < 0) {
		pr_err("[TCP server] : Fail: socket()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	saddr_s.sin_port = htons(port);

	if (bind(fd_listener, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) < 0) {
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
	unsigned int len_saddr;
	char	buf[40];	/* small buf */
	struct sockaddr_in	saddr_c;
	struct timeval tv_timeo = { 2, 500000};

	len_saddr = sizeof(struct sockaddr);
	for(;;) {
		cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr);
		if (cfd == -1) {
			printf("[Child] : Fail: accept()\n");
			close(cfd);
			continue;
		}
		printf("[Child:%d] Accept about socket(%d)\n", idx, cfd);

		/* receive time out with 5 sec */
		if (setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeo, sizeof(tv_timeo)) == -1) {
			fprintf(stderr, "[Child:%d] Fail: setsockopt(),fd(%d),(%s)\n", idx, cfd,strerror(errno));
			exit(EXIT_FAILURE);
		}

		for(;;) {
			ret_len = recv(cfd, buf, sizeof(buf), 0);
			if (ret_len < 0) {
				if (errno == EINTR) continue;
				if (errno == EAGAIN) {
					printf("[Child:%d] recv() get timeout error: %s\n", idx, strerror(errno));
					continue; /* time out */
				}
				fprintf(stderr, "[Child:%d] Fail: recv(): %s\n", idx, strerror(errno));
				break;
			}
			if (ret_len == 0) {
				fprintf(stderr, "[Child:%d] Session closed\n", idx);
				close(cfd);
				break;;
			}

			printf("[Child:%d] RECV(%.*s)\n", idx, ret_len, buf);
			if (send(cfd, buf, ret_len, 0) < 0) {
				fprintf(stderr, "[Child:%d] Fail: send() to socket(%d)\n", idx, cfd);
				close(cfd);
			}
			sleep(1); /* it's not special meaning */
		} /* packet recv loop */
	} /* main for loop */
}
