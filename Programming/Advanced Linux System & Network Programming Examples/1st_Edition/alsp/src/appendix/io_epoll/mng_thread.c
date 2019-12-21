/* vim: set ts=4 sw=4: */
/* Filename    : mng_thread.c
 * Description : thread manager module
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "io_macro.h"
#include "io_epoll_pthread.h"
#include "sysv_msg.h"

volatile struct targ_listener 	Targ_listener;
volatile struct targ_poller 	Targ_poller;
volatile struct targ_iosched 	Targ_iosched;
volatile struct targ_worker 	Targ_worker[MAX_WORKER_THREAD];

void *start_listener(void *t_arg);	/* listener thread	*/
void *start_poller(void *t_arg);	/* poller thread	*/
void *start_iosched(void *t_arg);	/* I/O scheduler	*/
void *start_worker(void *t_arg);	/* worker thread	*/

#define LISTEN_BACKLOG		20

/* Name : start_listener
 * Desc : make listener socket, accept client, add socket to epoll
 * Argv : 
 * Ret  : 
 */
void *start_listener(void *t_arg)
{
	struct targ_listener *p_arg = (struct targ_listener *) t_arg;
	int		fd_client;
	socklen_t len_saddr;
	struct sockaddr_in	saddr_c;

	/* start lisenter socket */
	p_arg->fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (p_arg->fd_listener < 0) {
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
	pr_out("Port : #%d\n", ntohs(p_arg->saddr_s.sin_port));
	listen(p_arg->fd_listener, LISTEN_BACKLOG);

	/* If you have multiple listener socket, you have to use accept serialization (poller with non-blocking).
	   Accept serialization will be eliminated overhead under race condition with multiple forked process.
	 */
	while(1) {
		fd_client = accept(p_arg->fd_listener, (struct sockaddr *)&saddr_c, &len_saddr);
		if (fd_client == -1) {
			pr_err("Error get connection from listen socket");
			/* need error processing */
			break;
		}
		pr_out("Add socket (%d)", fd_client);
		ADD_EFD(epollfd, fd_client);
		ADD_FDLIST(fd_client);
	}
	return p_arg;
}

/* Name : start_poller
 * Desc : When are connections, polling about client sockets. If not waiting on CV.
 * Argv : 
 * Ret  : 
 */
void *start_poller(void *t_arg)
{
	struct targ_poller *p_arg = (struct targ_poller *) t_arg;
	/* I/O multiplexing var */
	int		i, ret_poll;
	int		a_mq_buf[MAX_EVENT_QUEUE + 1];	/* sending buf to msq */

	while (1) {
		if (MUTEX_LOCK(p_conninfo) == -1) {
			pr_err("[ERR] MUTEX: Lock"); exit(EXIT_FAILURE);
		}
		/* waiting for data (no timeout) */
		if (p_conninfo->cur_open_files == 0) {
			printf("Epoll waiting for CV ...\n");
			if (CV_WAIT(p_conninfo) == -1) {
				pr_err("[ERR] CV: Wait"); exit(EXIT_FAILURE);
			}
			pr_out("Wake up poller thread");
		}
		if (MUTEX_UNLOCK(p_conninfo) == -1) {
			pr_err("[ERR] MUTEX: Lock"); exit(EXIT_FAILURE);
		}
		pr_out("EPoll : current # of fds (%d)", p_conninfo->cur_open_files);
		ret_poll = epoll_wait(epollfd, ep_events, p_conninfo->cur_open_files, -1);
		if (ret_poll == -1) {	/* error */
			pr_err("[ERR] EPOLL: epoll_wait()"); exit(EXIT_FAILURE);
		}

		/* require to check overflow about a_mq_buf[] */
		for (i=0; i<ret_poll; i++) {
			if (ep_events[i].events & EPOLLIN) {
				/* queueing */
				a_mq_buf[i+1] = ep_events[i].data.fd;
			} else if (ep_events[i].events & EPOLLERR) {
				/* error */
			} else if (ep_events[i].events & EPOLLHUP) {
				/* reconnect */
			} else {
				pr_err("fd(%d) epoll event(%d) Error(%d:%s)", ep_events[i].data.fd, ep_events[i].events, errno, strerror(errno));
			}
		}
		p_arg->n_epevent += ret_poll;

		/* send */
		a_mq_buf[0] = MQ_INTERNAL;	/* internal routing type */
		if (msgsnd(msg_id, (struct msgbuf *)a_mq_buf, sizeof(int) * i, IPC_NOWAIT)) {
			pr_err("[ERR] msgsnd()");
		}
		pr_out("+ MSQ Send (%d) item", i);
	}
	return p_arg;
}

/* Name : start_iosched
 * Desc : I/O scheduling
 * Argv : 
 * Ret  : 
 */
void *start_iosched(void *t_arg)
{
	struct targ_iosched *p_arg = (struct targ_iosched *) t_arg;
	int		i, n_ev;
	int		a_mq_buf[MAX_EVENT_QUEUE + 1];	/* sending buf to msq */
	int		idx_rr = 0;		/* round robin index */

	while (1) {
		/* start I/O scheduling */
		if ((n_ev = msgrcv(msg_id, (struct msgbuf *)a_mq_buf, 
						MAX_EVENT_QUEUE*sizeof(int), MQ_INTERNAL, 0)) == -1) {
			pr_err("[ERR] msgrcv()");
		}

		if (n_ev == 0) {
			continue;
		}
		n_ev /= sizeof(int);
		pr_out("Recv (%d) events", n_ev);

		/* distribute I/O event */
		for (i=0; i < n_ev; i++) {
			/* round robin */
			if (idx_rr == MAX_WORKER_THREAD) idx_rr = 0;

			if (push_evlist(p_evlists[idx_rr++], a_mq_buf[i+1])) {
				pr_err("[ERR] push_evlist()");
			}
		}
	}

	return p_arg;
}

/* Name : start_worker
 * Desc : worker thread
 * Argv : 
 * Ret  : 
 */
void *start_worker(void *t_arg)
{
	struct targ_worker *p_arg = (struct targ_worker *) t_arg;
	EVLIST		*p_evl = p_evlists[p_arg->no];
	FDLIST		*p_fdl;
	int			i, n_ev, n_recv;

	while (1) {
		p_arg->status = T_IDLE;

		/* start worker with mutex lock */
		if (MUTEX_LOCK(p_evl) == -1) {
			pr_err("[ERR] MUTEX: Lock"); exit(EXIT_FAILURE);
		}
		if (p_evl->n_ev == 0) {
			if (CV_WAIT(p_evl) < 0) {
				pr_err("[ERR] CV: Wait"); exit(EXIT_FAILURE);
			}
		}
		n_ev = p_evl->n_ev;
		if (MUTEX_UNLOCK(p_evl) == -1) {
			pr_err("[ERR] MUTEX: UnLock"); exit(EXIT_FAILURE);
		}

		p_arg->status = T_RUNNING;
		for (i=0; i<p_evl->n_ev; i++) {
			p_fdl = get_fdlistbyfd(pop_evlist(p_evl));
			if ((n_recv = recv(p_fdl->fd, p_fdl->rbuf, sizeof(p_fdl->rbuf), 0)) == -1) {
				pr_err("[TID:%lu] fd(%d) recv() error (%s)", p_arg->pt_id, p_fdl->fd, strerror(errno));
			} else {
				if (n_recv == 0) {
					/* closed */
					pr_out("[TID:%lu] fd(%d) CLOSED", p_arg->pt_id, p_fdl->fd);
					DEL_EFD(epollfd, p_fdl->fd);
					DEL_FDLIST(p_fdl->fd);
				} else {
					/* normal */
					pr_out("[TID:%lu] fd(%d) RECV:%d B", p_arg->pt_id, p_fdl->fd, n_recv);
				}
			}
		}

		if (MUTEX_LOCK(p_evl) == -1) {
			pr_err("[ERR] MUTEX: Lock"); exit(EXIT_FAILURE);
		}
		/* KH code: here there is a logical bug; Updating the number of event is coincident with the function 'pop_evlist()' */
		//p_evl->n_ev -= n_ev;	/* update number of event */
		pr_out(" ------- n_ev: %d \n", p_evl->n_ev);
		if (MUTEX_UNLOCK(p_evl) == -1) {
			pr_err("[ERR] MUTEX: UnLock"); exit(EXIT_FAILURE);
		}
	}

	return p_arg;
}

