/* vim: set ts=4 sw=4: */
/* Filename    : io_epoll.c
 * Description : I/O multiplexing implementation with epoll
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : when define ENABLE_MSG_OOB, it allow to receive OOB data
 */
#include "io_macro.h"
#include "type_macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define USAGE	"Usage: %s [listen port]\n"
#define LISTEN_BACKLOG	20
#define DFL_OPEN_MAX_FILES	8192		/* Default value for epoll */

#define ADD_EV(a, b)	if (add_ev(a, b) == -1) { pr_err("Fail: add_ev"); exit(EXIT_FAILURE); }
#define DEL_EV(a, b)	if (del_ev(a, b) == -1) { pr_err("Fail: del_ev"); exit(EXIT_FAILURE); }

int	epollfd;		/* epoll fd */

int add_ev(int efd, int fd);	/* Add epoll variable to epollfd */
int del_ev(int efd, int fd); /* delete epoll variabe from epollfd */

int main(int argc, char *argv[])
{
	/* network var			*/
	short	port;
	socklen_t	len_saddr;
	struct sockaddr_in	saddr_s, saddr_c;
	int		fd;
	int		fd_listener;
	int		n_recv, n_buffer;
	char	buf_recv[1024];
	/* I/O multiplexing var */
	struct epoll_event	*ep_events;
	int		max_open_files;
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
	if (argc > 2) {
		printf(USAGE, argv[0]);
		exit(EXIT_FAILURE);
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

	/* use nonblocking socket for edge trigger interface */
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

	max_open_files = MIN((int)sysconf(_SC_OPEN_MAX), (int)DFL_OPEN_MAX_FILES);
	if ((epollfd = epoll_create(max_open_files)) == -1) {
		/* error */
		exit(EXIT_FAILURE);
	}
	if ((ep_events = calloc(1024, sizeof(struct epoll_event))) == NULL) {
		/* error */
		exit(EXIT_FAILURE);
	}
	pr_out("Epoll create with %d max files", max_open_files);

	ADD_EV(epollfd, fd_listener);
	while (1) {

		/* waiting for data (no timeout) */
		pr_out("Epoll waiting ...");
		ret_poll = epoll_wait(epollfd, ep_events, max_open_files, -1);
		if (ret_poll == -1) {
			/* error */
		}
		pr_out("EPoll return (%d)", ret_poll);


		/* Are there data to read? */
		for (i=0; i<ret_poll; i++) {
#ifdef ENABLE_MSG_OOB
			if (ep_events[i].events & EPOLLPRI) {
				/* Urgent data received */
				pr_out("Urgent data detected");
				for( ; ; ) { /* It dosen't good idea using infinite loop to read data. But it just example. */
					if (n_tot_recv > sizeof(buf_recv)) {
						/* buffer overflow but I don't care. It just example */
						printf("ERR: buffer overflow detected\n"); exit(EXIT_FAILURE);
					}
					ret = ioctl(ep_events[i].data.fd, SIOCATMARK, &flag_atmark);
					/* assuming OOB data arrived, If flag_atmark return 1, next recv will read OOB.
					   Otherwise, the next read will provide data send by the client prior to transmission
					   of the OOB signal */
					if (ret == -1) {
						pr_err("Error set mark to socket");
					}
					if (flag_atmark == 1) {
						n_recv = recv(ep_events[i].data.fd, buf_recv+n_tot_recv, 1, MSG_OOB);
						if (n_recv == -1) {
							printf("ERR: OOB:n_recv (%d)\n", n_recv);
						}
						n_tot_recv++;
						printf(">> Data(with OOB)=[%.*s]\n", n_tot_recv, buf_recv);
						n_tot_recv = 0;
						break;
					} else {
						n_recv = recv(ep_events[i].data.fd, buf_recv+n_tot_recv, sizeof(buf_recv)-n_tot_recv, 0);
						if (n_tot_recv == -1) {
							pr_err("fd(%d) recv() error (%s)", ep_events[i].data.fd, strerror(errno));
						}
						n_tot_recv += n_recv;
					}
				}
			} else 
#endif
			if (ep_events[i].events & EPOLLIN) {
				/* IS New connection ? */
				if (ep_events[i].data.fd == fd_listener) {
					while(1) {
						fd = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
						if (fd == -1) {
							if (errno == EAGAIN) { /* no more new connection */
								break;
							}
							pr_err("Error get connection from listen socket");
							/* need error processing */
							break;
						}
						ADD_EV(epollfd, fd);
						pr_out("Add socket (%d)", fd);
					}
					continue;
				}

				/* check recv socket buffer */
				if (ioctl(ep_events[i].data.fd, FIONREAD, &n_buffer) == -1) {
					pr_err("ioctl(%d, FIONREAD, ..) (%s)", ep_events[i].data.fd, strerror(errno));
				} else {
					pr_out("fd(%d) %d byte are received on socket buffer", ep_events[i].data.fd, n_buffer);
				}

				if ((n_recv = recv(ep_events[i].data.fd, buf_recv, sizeof(buf_recv), 0)) == -1) {
					pr_err("fd(%d) recv() error (%s)", ep_events[i].data.fd, strerror(errno));
				} else {
					if (n_recv == 0) {
						/* closed */
						pr_out("fd(%d) was closed by foriegn host", ep_events[i].data.fd);
						DEL_EV(epollfd, ep_events[i].data.fd);
					} else {
						/* normal */
						pr_out("fd(%d) %d bytes received", ep_events[i].data.fd, n_recv);
					}
				}
			} else if (ep_events[i].events & EPOLLERR) {
				/* error */
			} else {
				pr_out("fd(%d) epoll event(%d) Error(%d:%s)", ep_events[i].data.fd, ep_events[i].events, errno, strerror(errno));
			}
		}
	}

	return 0;
}

/* Name : add_ev
 * Desc : add socket descriptor to epoll
 * Argv : int efd, int fd
 * Ret  : zero(if success), negative(if fail)
 */
int add_ev(int efd, int fd)
{
	struct epoll_event ev;

	/* if want to use edge trigger, add EPOLLET to events */
#ifdef ENABLE_MSG_OOB	
	ev.events = EPOLLIN | EPOLLPRI;	/* check inbound data */
#else
	ev.events = EPOLLIN ;	/* check inbound data */
#endif
	ev.data.fd = fd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		pr_out("fd(%d) EPOLL_CTL_ADD  Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}

	return 0;
}

/* Name : del_ev
 * Desc : delete socket descriptor from epoll
 * Argv : int efd, int fd
 * Ret  : zero(if success), negative(if fail)
 */
int del_ev(int efd, int fd)
{
	if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		pr_out("fd(%d) EPOLL_CTL_DEL Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}
	close(fd);
	return 0;
}
