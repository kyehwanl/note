/* vim: set ts=4 sw=4: */
/* Filename    : io_select2.c
 * Description : I/O multiplexing implementation with select()
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : listener to be nonblocking mode
 */
#include "io_macro.h"
#include "type_macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define USAGE	"%s <listen port>\n"
#define LISTEN_BACKLOG	20
#define MAX_FD_SOCKET	0xff

int		fd_socket[MAX_FD_SOCKET];
int		cnt_fd_socket;

/* I/O multiplexing var */
fd_set	fds_read;
int		fd_biggest;

int add_socket(int fd);
int del_socket(int fd);
int mk_fds(fd_set *fds, int *a_fd_socket);

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
	int		ret_select;
	/* logical var 			*/
	int		i;

	/* init variables */
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));
	memset(&saddr_c, 0x00, sizeof(struct sockaddr_in));
	for (i=0; i<MAX_FD_SOCKET; i++) {
		fd_socket[i] = -1;
	}

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
		pr_err("Fail: socket()"); exit(EXIT_FAILURE);
	}
	if (fcntl(fd_listener, F_SETFL, O_NONBLOCK) == -1) { /* nonblocking */
		pr_err("Fail: fcntl()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost any address */
	saddr_s.sin_port = htons(port);

	if (bind(fd_listener, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("Fail: bind()"); exit(EXIT_FAILURE);
	}

	len_saddr = sizeof(saddr_s);
	if (port == 0) {
		getsockname(fd_listener, (struct sockaddr *)&saddr_s, &len_saddr);
	}
	pr_out("Port : #%d\n", ntohs(saddr_s.sin_port));

	listen(fd_listener, LISTEN_BACKLOG);

	/* I/O Multiplexing */
	FD_ZERO(&fds_read);
	add_socket(fd_listener);

	while (1) {
		/* waiting for data (no timeout) */
		fd_biggest = mk_fds(&fds_read, fd_socket);
		ret_select = select(fd_biggest+1, &fds_read, NULL, NULL, NULL);
		if (ret_select == -1) {
			/* error */
		}
		pr_out("Select return (%d)", ret_select);

		/* IS New connection ? */
		if (FD_ISSET(fd_listener, &fds_read)) {
			while (1) {
				/* Accept only one connection allowed per cycle */
				fd = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
				if (fd == -1) {
					if (errno == EAGAIN) {
						pr_out("No more exist new connection");
						break;
					}
					pr_err("Error get connection from listen socket");
				}
				if (add_socket(fd) == -1) {
					/* error : too many client connected. force to disconnect. */
				}
				pr_out("Add socket (%d)", fd);
			}
			continue;
		}

		/* Are there data to read? */
		for (i=1; i<cnt_fd_socket; i++) {
			if (FD_ISSET(fd_socket[i], &fds_read)) {
				/* check recv socket buffer */
				if (ioctl(fd_socket[i], FIONREAD, &n_buffer) == -1) {
					pr_err("ioctl(%d, FIONREAD, ..) (%s)", fd_socket[i], strerror(errno));
				} else {
					pr_out("fd(%d) %d byte are received on socket buffer", fd_socket[i], n_buffer);
				}

				if ((n_recv = recv(fd_socket[i], buf_recv, sizeof(buf_recv), 0)) == -1) {
					pr_err("fd(%d) recv() error (%s)", fd_socket[i], strerror(errno));
				} else {
					if (n_recv == 0) {
						/* closed */
						pr_out("fd(%d) was closed by foriegn host", fd_socket[i]);
						del_socket(fd_socket[i]);
					} else {
						/* normal */
						pr_out("fd(%d) %d bytes received", fd_socket[i], n_recv);
					}
				}
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
		fd_socket[cnt_fd_socket] = fd;
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
		if (fd_socket[i] == fd) {
			if (i != (cnt_fd_socket-1)) fd_socket[i] = fd_socket[cnt_fd_socket-1];
			fd_socket[cnt_fd_socket-1] = -1;
			flag = 1;
		}
	}

	if (flag == 0) {
		return -1;
	}
	--cnt_fd_socket;

	return i;
}

/* Name : mk_fds
 * Desc : make fd set
 * Argv : fd_set *fds, int *a_fd_socket
 * Ret  : positive value (means biggest fd number)
 */
int mk_fds(fd_set *fds, int *a_fd_socket)
{
	int	i, fd_max;
	FD_ZERO(fds);
	for (i=0, fd_max = -1; i<cnt_fd_socket; i++) {
		fd_max = MAX(fd_max, a_fd_socket[i]);
		FD_SET(a_fd_socket[i], fds);
	}
	return fd_max;
}
