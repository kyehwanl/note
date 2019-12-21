/* vim: set ts=4 sw=4: */
/* Filename    : mng_io.c
 * Description : io management module
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include "io_macro.h"
#include "io_epoll_pthread.h"

int add_efd(int efd, int fd);
int del_efd(int efd, int fd);
int push_evlist(EVLIST *evl, int fd);
int pop_evlist(EVLIST *evl);
inline FDLIST * get_fdlistbyfd(int fd);

/* Name : add_efd
 * Desc : add socket descriptor to epoll
 * Argv : int efd, int fd
 * Ret  : zero(if success), negative(if fail)
 */
int add_efd(int efd, int fd)
{
	struct epoll_event ev;

	if (MUTEX_LOCK(p_conninfo) == -1) {
		pr_err("[ERR] MUTEX: Lock"); exit(EXIT_FAILURE);
	}

	if (p_conninfo->cur_open_files >= p_conninfo->max_open_files) {
		/* overflow error, required close socket & related cleaning job */
		if (MUTEX_UNLOCK(p_conninfo) == -1) {
			pr_err("[ERR] MUTEX: Lock"); exit(EXIT_FAILURE);
		}
		return -1;
	}

	ev.events = EPOLLIN | EPOLLET;	/* check inbound data with edge trigger */
	ev.data.fd = fd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		pr_err("fd(%d) EPOLL_CTL_ADD  Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}
	p_conninfo->cur_open_files++;
	if (p_conninfo->cur_open_files == 1) { /* send signal */
		if (CV_SIGNAL(p_conninfo) == -1) {
			pr_err("[ERR] CV: Signal"); 
			exit(EXIT_FAILURE);
		}
	}

	if (MUTEX_UNLOCK(p_conninfo) == -1) {
		pr_err("[ERR] MUTEX: Lock"); 
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* Name : del_efd
 * Desc : delete socket descriptor from epoll
 * Argv : int efd, int fd
 * Ret  : zero(if success), negative(if fail)
 */
int del_efd(int efd, int fd)
{
	if (MUTEX_LOCK(p_conninfo) == -1) {
		pr_err("[ERR] MUTEX: Lock"); 
		exit(EXIT_FAILURE);
	}

	if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		pr_err("fd(%d) EPOLL_CTL_DEL Error(%d:%s)", fd, errno, strerror(errno));
		return -1;
	}
	close(fd);
	p_conninfo->cur_open_files--;

	if (MUTEX_UNLOCK(p_conninfo) == -1) {
		pr_err("[ERR] MUTEX: Lock"); 
		exit(EXIT_FAILURE);
	}
	return 0;
}


/* Name : push_evlist
 * Desc : 
 * Argv : 
 * Ret  : zero(if success), negative(if fail)
 */
int push_evlist(EVLIST *evl, int fd)
{
	if (MUTEX_LOCK(evl) == -1) {
		pr_err("[ERR] MUTEX: Lock"); 
		exit(EXIT_FAILURE);
	}

	if (evl->n_ev == MAX_EVENT_QUEUE) { /* check overflow */
		return -1;
	}
	if (evl->i2_ev == MAX_EVENT_QUEUE) { /* recalculate index */
		evl->i2_ev = 0;
	}

	evl->q_ev[evl->i2_ev++] = fd;	/* insert fd 		*/
	evl->n_ev++;					/* increase event 	*/

	/* send signal when worker thread wait on CV */
	if (evl->n_ev == 1) {
		if (CV_SIGNAL(evl) == -1) {
			pr_err("[ERR] MUTEX: Unlock"); 
			exit(EXIT_FAILURE);
		}
	}

	pr_out("EVL(start~end/total :%d~%d/%d)", evl->i1_ev, evl->i2_ev, evl->n_ev);
	if (MUTEX_UNLOCK(evl) == -1) {
		pr_err("[ERR] MUTEX: Unlock"); 
		exit(EXIT_FAILURE);
	}
	return 0;
}

/* Name : pop_evlist
 * Desc : 
 * Argv : 
 * Ret  : positive(if success), negative(if fail)
 */
int pop_evlist(EVLIST *evl)
{
	int		ret;
	if (MUTEX_LOCK(evl) == -1) {
		pr_err("[ERR] MUTEX: Lock"); 
		exit(EXIT_FAILURE);
	}

	if (evl->n_ev == 0) { /* check underflow */
		return -1;
	}

	ret = evl->q_ev[evl->i1_ev++];
	evl->n_ev--;		/* decrease event 	*/
	printf(" === evl->n_ev: %d \n", evl->n_ev);

	if (MUTEX_UNLOCK(evl) == -1) {
		pr_err("[ERR] MUTEX: Unlock"); 
		exit(EXIT_FAILURE);
	}
	return ret;
}


/* Name : get_fdlistbyfd
 * Desc : get 
 * Argv : 
 * Ret  : positive(if success), negative(if fail)
 */
inline FDLIST * get_fdlistbyfd(int fd)
{
	return (&p_fdlists[fd]);
}
