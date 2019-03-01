/* vim: set ts=4 sw=4: */
/* Filename    : io_poll.c
 * Description : I/O multiplexing implementation with poll()
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : when define ENABLE_MSG_OOB, it allow to receive OOB data
 */
#include "io_macro.h"
#include "type_macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define USAGE	"Usage: %s <listen port>\n"
#define LISTEN_BACKLOG	20
#define MAX_FD_SOCKET	0xff

struct pollfd pollfds[MAX_FD_SOCKET];
int		cnt_fd_socket;

int add_socket(int fd);
int del_socket(int fd);
void pr_socket();

int main(int argc, char *argv[])
{
	/* network var			*/
	short	port;
	socklen_t		len_saddr;
	struct sockaddr_in	saddr_s, saddr_c;
	int		fd;
	int		fd_listener;
	int		n_recv, n_buffer;
	char	buf_recv[1024];
	/* I/O multiplexing var */
	int		ret_poll;
	/* logical var 			*/
	int		i;
#ifdef ENABLE_MSG_OOB
	int		ret, flag_atmark, n_tot_recv=0;
#endif

	/* init variables */
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));
	memset(&saddr_c, 0x00, sizeof(struct sockaddr_in));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc != 2) {
		printf(USAGE, argv[0]);
	}
	if (argc == 2) {
		port = atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	pr_out("Process Start");
	fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (fd_listener == -1) {
		pr_err("Fail: socket()");
		exit(EXIT_FAILURE);
	}

	/* use nonblocking socket */
	if (fcntl(fd_listener, F_SETFL, O_NONBLOCK) == -1) {	/* only influence about O_ASYNC, O_APPEND, O_NONBLOCK */
		/* error */
		exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	saddr_s.sin_port = htons(port);

	if (bind(fd_listener, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("Fail: bind()");
		exit(EXIT_FAILURE);
	}

	len_saddr = sizeof(saddr_s);
	if (port == 0) {
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
	}
	pr_out("Port : #%d", ntohs(saddr_s.sin_port));

	listen(fd_listener, LISTEN_BACKLOG);
	add_socket(fd_listener);

	while (1) {
		/* waiting for data (no timeout) */
		ret_poll = poll(pollfds, cnt_fd_socket, -1);
		if (ret_poll == -1) {
			/* error */
		}
		pr_out("poll return (%d)", ret_poll);

		/* Is New connection ? */
		if (pollfds[0].revents & POLLIN) {
			while (1) {
				fd = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
				if (fd == -1) {
					if (errno == EAGAIN) {
						break;
					}
					pr_err("Error get connection from listen socket");
					/* neeed error processing */
					break;
				}
				if (add_socket(fd) == -1) {
					/* error : too many client connected. force to disconnect. */
				}
				pr_out("Add socket (%d)", fd);
			}
			continue;
		}
		/* Are there data to read? */
		for (i=1; i<cnt_fd_socket && ret_poll > 0; i++) {
#ifdef ENABLE_MSG_OOB
			if (pollfds[i].revents & POLLPRI) {
				printf("Urgent data detected\n");
				for( ; ; ) { /* It dosen't good idea using infinite loop to read data. But it just example. */
					if (n_tot_recv > sizeof(buf_recv)) {
						/* buffer overflow but I don't care. It just example */
						printf("ERR: buffer overflow detected\n"); exit(EXIT_FAILURE);
					}
					ret = ioctl(pollfds[i].fd, SIOCATMARK, &flag_atmark);
					/* assuming OOB data arrived, If flag_atmark return 1, next recv will read OOB.
					   Otherwise, the next read will provide data send by the client prior to transmission
					   of the OOB signal */
					if (ret == -1) {
						pr_err("Error set mark to socket");
					}
					if (flag_atmark == 1) {
						n_recv = recv(pollfds[i].fd, buf_recv+n_tot_recv, 1, MSG_OOB);
						if (n_recv == -1) {
							printf("ERR: OOB:n_recv (%d)\n", n_recv);
						}
						n_tot_recv++;
						printf("Data(with OOB)=[%.*s]\n", n_tot_recv, buf_recv);
						n_tot_recv = 0;
						break;
					} else {
						n_recv = recv(pollfds[i].fd, buf_recv+n_tot_recv, sizeof(buf_recv)-n_tot_recv, 0);
						if (n_recv == -1) {
							pr_err("fd(%d) recv() error (%s)", pollfds[i].fd, strerror(errno));
						}
						n_tot_recv += n_recv;
					}
				}
			} else
#endif
			if (pollfds[i].revents & POLLIN) {
				/* check recv socket buffer */
				if (ioctl(pollfds[i].fd, FIONREAD, &n_buffer) == -1) {
					pr_err("ioctl(%d, FIONREAD, ..) (%s)", pollfds[i].fd, strerror(errno));
				} else {
					pr_out("fd(%d) %d byte are received on socket buffer", pollfds[i].fd, n_buffer);
				}

				if ((n_recv = recv(pollfds[i].fd, buf_recv, sizeof(buf_recv), 0)) == -1) {
					pr_err("fd(%d) recv() error (%s)", pollfds[i].fd, strerror(errno));
				} else {
					if (n_recv == 0) {
						/* closed */
						pr_out("fd(%d) was closed by foriegn host", pollfds[i].fd);
						del_socket(pollfds[i].fd);
						i--;	/* becoz, check rearranged socket position by del_socket() */
					} else {
						/* normal */
						pr_out("fd(%d) %d bytes received", pollfds[i].fd, n_recv);
					}
				}
				ret_poll--;
			} else if (pollfds[i].revents & POLLERR) {
				/* error */
				ret_poll--;
			} else if (pollfds[i].revents & POLLNVAL) {
				/* invalid fd */
				ret_poll--;
			} else {
				pr_out("> No signal:fd(%d)", pollfds[i].fd);
			}
		}
	}

	return 0;
}

/* Name : add_socket
 * Desc : add socket descriptor to internal fd array
 * Argv : int fd
 * Ret  : positive(if success, means array index), negative(if fail)
 */
int add_socket(int fd)
{
	if (cnt_fd_socket < MAX_FD_SOCKET) {
		pollfds[cnt_fd_socket].fd = fd;
#ifdef ENABLE_MSG_OOB
		pollfds[cnt_fd_socket].events = POLLIN | POLLPRI;
#else
		pollfds[cnt_fd_socket].events = POLLIN;
#endif
		return ++cnt_fd_socket;
	} else {
		return -1;
	}
}

/* Name : del_socket
 * Desc : substract socket descriptor from internal fd array
 * Argv : int fd
 * Ret  : positive(if success, means array index), negative(if fail)
 */
int del_socket(int fd)
{
	int	 i, flag;
	flag = 0; /* 1:found, 0:not found */

	close(fd);
	for (i=0; i<cnt_fd_socket; i++) {
		if (pollfds[i].fd == fd) {
			if (i != (cnt_fd_socket-1)) { 
				pollfds[i] = pollfds[cnt_fd_socket-1];
			}
			pollfds[cnt_fd_socket-1].fd = -1;
			flag = 1;
		}
	}

	if (flag == 0) {
		return -1;
	}
	--cnt_fd_socket;

	return i;
}

/* Name : pr_socket
 * Desc : print socket list in poller
 * Argv : None
 * Ret  : None
 */
void pr_socket()
{
	int	i;
	printf("> Count of socket list = %d\n", cnt_fd_socket);
	for (i=0; i<cnt_fd_socket; i++) {
		pr_out("%d) fd(%d)", i, pollfds[i].fd);
	}
}
