/* vim: set ts=4 sw=4: */
/* Filename    : io_rts.c
 * Description : RTS
 * Author      : Sun Young Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include "io_macro.h"
#include "type_macro.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <poll.h>

#define USAGE	"Usage: %s [listen port]\n"
#define LISTEN_BACKLOG	20

#define SZ_RECV_BUF		16784

#define SIGRT_LISTEN_IO		SIGRTMIN
#define SIGRT_NORM_IO		SIGRTMIN + 1

/* RTS */
int	add_rts_socket(int	fd, int	i_sig);
int start_sigrt(void *t_arg);

struct sock_arg {
	struct sockaddr_in	saddr_s;
	int		fd_listener;
} Sock_arg;

int main(int argc, char *argv[])
{
	int		port;
	/* logical var 			*/
	sigset_t	sigset_mask;

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc > 2) {
		printf(USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	/* blocking signal */
	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGRT_LISTEN_IO);
	sigaddset(&sigset_mask, SIGRT_NORM_IO);
	if (sigprocmask(SIG_BLOCK, &sigset_mask, NULL) == -1) {
		exit(EXIT_FAILURE);
	}
	

	pr_out("Process Start");
	/* setting socket address structure */
	Sock_arg.saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	Sock_arg.saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	Sock_arg.saddr_s.sin_port = htons(port);


	if (start_sigrt(&Sock_arg) == -1) { /* error */
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* Name : start_sigrt
 * Desc : 
 * Argv : 
 * Ret  : zero(if success), negative(if fail)
 */

int start_sigrt(void *t_arg)
{
	struct sock_arg *p_arg = (struct sock_arg *) t_arg;
	int		fd_client;
	struct sockaddr_in	saddr_c;
	socklen_t len_saddr;
	char	rbuf[SZ_RECV_BUF];
	int		n_recv;
	/* for RTS */
	int		i_sig;
	sigset_t	sigset_mask;
	siginfo_t	siginfo_rt;
	struct timespec tspec = { 5, 0};

	/* start lisenter socket */
	p_arg->fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (p_arg->fd_listener == -1) {
		pr_err("Fail: socket()");
		exit(EXIT_FAILURE);
	}

	if (bind(p_arg->fd_listener, (struct sockaddr *)& p_arg->saddr_s, sizeof(struct sockaddr_in)) == -1) {
		pr_err("Fail: bind()");
		exit(EXIT_FAILURE);
	}

	if (p_arg->saddr_s.sin_port == 0) {
		len_saddr = sizeof(struct sockaddr_in);
		getsockname(p_arg->fd_listener, (struct sockaddr *)&p_arg->saddr_s, &len_saddr);
	}
	pr_out("Port : #%d", ntohs(p_arg->saddr_s.sin_port));
	listen(p_arg->fd_listener, LISTEN_BACKLOG);
	add_rts_socket(p_arg->fd_listener, SIGRT_LISTEN_IO);

	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGRT_LISTEN_IO);
	sigaddset(&sigset_mask, SIGRT_NORM_IO);

	/* raise SIGIO when RT signal queue overflows */
	while (1) {
		i_sig = sigtimedwait(&sigset_mask, &siginfo_rt, &tspec);
		if (i_sig == SIGRT_LISTEN_IO) {
			fd_client = accept(p_arg->fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
			if (fd_client == -1) {
				if (errno == EAGAIN) { /* no more new connection */
					break;
				}
				pr_err("Error get connection from listen socket");
				/* need error processing */
				break;
			}
			pr_out("[SIGRT] Add socket (%d)", fd_client);

			/* new client */
			add_rts_socket(fd_client, SIGRT_NORM_IO);

		} else if (i_sig == SIGRT_NORM_IO) {
			if (siginfo_rt.si_band & POLLIN) {
				n_recv = recv(siginfo_rt.si_fd, rbuf, sizeof(rbuf), 0);
				if (n_recv > 0) {
					/* success */
					printf("[SIGRT] Recv(%d:%.*s)\n", n_recv, n_recv, rbuf);
				} else {
					if (n_recv == 0) {
						/* closed */
						printf("[SIGRT] close fd(%d)\n", siginfo_rt.si_fd);
						close(siginfo_rt.si_fd);
					}
					if (n_recv == -1) {
						/* error */
						printf("[SIGRT] ERR: recv(%d)\n", errno);
					}
				}
			} else if (siginfo_rt.si_band & POLLERR) {
				printf("[SIGRT] POLLERR\n");
			} else if (siginfo_rt.si_band & POLLHUP) {
				printf("[SIGRT] POLLHUP\n");
			} else {
				/* Other signal band */
				printf("[SIGRT] Unknown band\n");
			}
		} else {
			pr_out("Unknown signal or timeout : %d", i_sig);
		}
	}

	return 0;
}

int	add_rts_socket(int	fd, int	i_sig)
{
	int	flags;
	if ((flags = fcntl(fd, F_GETFL)) == -1) {
		pr_err("[ERR] fail : fcntl() F_GETFL"); return -1;
	}
	if (fcntl(fd, F_SETFL, flags|O_NONBLOCK|O_ASYNC) == -1) { /* basic flag */
		pr_err("[ERR] fail : fcntl() F_SETFL"); return -1;
	}
	if (fcntl(fd, F_SETSIG, i_sig) == -1) { /* Set signal to specified fd */
		pr_err("[ERR] fail : fcntl() F_SETSIG"); return -1;
	}
	if (fcntl(fd, F_SETOWN, getpid()) == -1) { /* set owner process information */
		pr_err("[ERR] fail : fcntl() F_SETOWN"); return -1;
	}
	return 0;
}
