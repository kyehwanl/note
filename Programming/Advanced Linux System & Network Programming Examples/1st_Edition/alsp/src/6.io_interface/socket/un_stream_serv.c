/* vim: set ts=4 sw=4: */
/* Filename    : un_stream_serv.c
 * Description : prefork child and wait for tcp client connection
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : prefork model of TCP echo server
 * Todo        : make routine for signal SIGCHLD
 */
#include "io_macro.h"
#include "fork_socket.h"
#include <sys/un.h>
#include <sys/wait.h>

#define PATH_UNIX_SOCKET    "/tmp/my_socket"
#define MAX_POOL	3

int fd_listener;
void start_child(int fd, int idx);

int main(int argc, char *argv[])
{
	int		i;
	short	port;
	char	ipaddr[15+1];
	struct sockaddr_un	saddr_s;
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

	fd_listener = socket(AF_UNIX, SOCK_STREAM, IPPROTO_IP);
	if (fd_listener == -1) {
		pr_err("[TCP server] : Fail: socket()"); exit(EXIT_FAILURE);
	}
	if (remove(PATH_UNIX_SOCKET)) {
		if (errno != ENOENT) { /* exit when occur error except "No such file.." */
			pr_err("[TCP Socket] : Fail: remove()"); exit(EXIT_FAILURE);
		}
	}

	/* setting socket address structure */
	saddr_s.sun_family = AF_UNIX;			/* AF_INET, PF_INET are same */
	sprintf(saddr_s.sun_path, PATH_UNIX_SOCKET);

	if (bind(fd_listener, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("[TCP server] : Fail: bind()"); exit(EXIT_FAILURE);
	}

	printf("[TCP server] : PATH : #%s\n", PATH_UNIX_SOCKET);
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

	len_saddr = sizeof(struct sockaddr);
	for(;;) {
		cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr);
		if (cfd == -1) {
			printf("[Child] : Fail: accept()\n");
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
