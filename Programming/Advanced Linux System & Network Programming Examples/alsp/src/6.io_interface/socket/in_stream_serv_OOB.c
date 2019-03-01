/* vim: set ts=4 sw=4: */
/* Filename    : in_stream_serv_OOB.c
 * Description : prefork child and wait for tcp client connection
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : prefork model of TCP echo server
 * Todo        : make routine for signal SIGCHLD
 */
#include "io_macro.h"
#include "fork_socket.h"
#include <sys/wait.h>
#include <sys/ioctl.h>

#define MAX_POOL	3

int fd_listener;
void start_child(int fd, int idx);

int main(int argc, char *argv[])
{
	int	i;
	socklen_t len_saddr;
	short	port;
	struct sockaddr_in	saddr_s;
	char	ipaddr[15+1];
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
	int		cfd, ret, n_tot_recv, n_recv;
	unsigned int len_saddr;
	struct sockaddr_in	saddr_c;
	char	buf_recv[0xff];
	int		flag_atmark; /* if OOB set to 1, otherwise 0 */

	n_tot_recv = n_recv = 0;

	len_saddr = sizeof(struct sockaddr);
	for(;;) {
		cfd = accept(sfd, (struct sockaddr *)&saddr_c, &len_saddr);
		if (cfd == -1) {
			printf("[Child] : Fail: accept()\n");
			continue;
		}
		printf("[Child:%d] Accept about socket(%d)\n", idx, cfd);

		for(;;) {
			ret = ioctl(cfd, SIOCATMARK, &flag_atmark);
			/* assuming OOB data arrived, If flag_atmark return 1, next recv will read OOB.
			   Otherwise, the next read will provide data send by the client prior to transmission
			   of the OOB signal */
			if (ret == -1) {
				pr_err("Error set mark to socket");
			}
			if (flag_atmark == 1) {
				n_recv = recv(cfd, buf_recv+n_tot_recv, sizeof(buf_recv)-n_tot_recv, MSG_OOB);
				if (n_recv == -1) {
					printf("ERR: OOB:n_recv (%d)\n", n_recv);
					goto WAIT_RECV;
				}
				n_tot_recv++;
				printf("Data(with OOB)=[%.*s]\n", n_tot_recv, buf_recv);
				n_tot_recv = 0;	/* init received data length */
			} else {
WAIT_RECV:
				n_recv = recv(cfd, buf_recv+n_tot_recv, sizeof(buf_recv)-n_tot_recv, 0);
				if (n_recv == -1) {
					pr_err("fd(%d) recv() error (%s)", cfd, strerror(errno));
				}

				if (n_recv == 0) {
					/* closed */
					pr_out("fd(%d) was closed by foriegn host", cfd);
					close(cfd);
					break;
				}
				n_tot_recv += n_recv;
			}
		} /* packet recv loop */
	} /* main for loop */
}
