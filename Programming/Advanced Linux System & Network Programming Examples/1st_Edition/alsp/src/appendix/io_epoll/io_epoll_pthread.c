/* vim: set ts=4 sw=4: */
/* Filename    : io_epoll_pthread.c
 * Description : epoll with pthread
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
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
#include <pthread.h>
#include "io_macro.h"
#include "type_macro.h"
#include "io_epoll_pthread.h"
#include "sysv_msg.h"

#define USAGE	"Usage: %s [listen port]\n"
#define MAX_THREAD_SLOT		10

/* epoll resources */
int		epollfd;
struct epoll_event	*ep_events;

/* SysV IPC: Message queue */
int		msg_id;

/* Connection information */
CONNINFO	*p_conninfo;
EVLIST		*p_evlists[MAX_WORKER_THREAD];	/* event list slot	*/
FDLIST		*p_fdlists;				/* client slot		*/

/* Thread slot */
PTHR_SLOT	pthr_slots[] = {
	{0, start_listener, (void *)&Targ_listener, sizeof(Targ_listener), "listener thread"},
	{0, start_poller,   (void *)&Targ_poller,   sizeof(Targ_poller),   "I/O poller thread"},
	{0, start_iosched,  (void *)&Targ_iosched,  sizeof(Targ_iosched),  "I/O scheduler thread"},
	{0, NULL, NULL, 0, NULL}
};


/* Function declaration */
int alloc_resources();
int make_threads();


int main(int argc, char *argv[])
{
	int		port;

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
	/* setting socket address structure */
	Targ_listener.saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	Targ_listener.saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	Targ_listener.saddr_s.sin_port = htons(port);

	/* init global variables */
	if (alloc_resources() || make_threads()) { /* error */
		exit(EXIT_FAILURE);
	}

	while(1) {
		/* load balancer in here!
		   There are two functions required.
		   (1) calculate periodic load about each thread
		   (2) evaluate weighted avg load to distribute work load

		   But, I have no time to make it.
		   There are nothing to do in here. Just sleeping 1 sec.
		 */
		sleep(1);
	}

	return 0;
}



/* Name : alloc_resources
 * Desc : allocate memory & other resource
 * Argv : 
 * Ret  : zero(if success), negative(if fail)
 */
int alloc_resources()
{
	int		i;
	/* make connection info structure */
	if ((p_conninfo = (CONNINFO *) calloc(1, sizeof(CONNINFO))) == NULL) {
		return -1;
	}
	p_conninfo->max_open_files = MIN((int)sysconf(_SC_OPEN_MAX), (int)DFL_OPEN_MAX_FILES);
	if (pthread_mutex_init(&p_conninfo->mutex_lock, NULL) < 0) { /* error */
		pr_out("[ERR] MUTEX: INIT"); exit(EXIT_FAILURE);
	}
	if (pthread_cond_init(&p_conninfo->cv_barrier, NULL) < 0) { /* error */
		pr_out("[ERR] CV: INIT"); exit(EXIT_FAILURE);
	}
	pr_out("Setup <CONNINFO>");

	/* allocate memory to fd list */
	if ((p_fdlists = (FDLIST *) calloc(DFL_OPEN_MAX_FILES, sizeof(FDLIST))) == NULL) {
		return -1;
	}
	pr_out("Setup <FDLIST>");

	/* allocate memory to event list */
	for (i=0; i<MAX_WORKER_THREAD; i++) {
		if ((p_evlists[i] = (EVLIST *) calloc(1, sizeof(EVLIST))) == NULL) {
			return -1;
		}
	}
	pr_out("Setup <EVLIST>");

	/* make message queue */
	if ((msg_id = sysv_msgget(NULL, MY_MSGKEY, 0664)) < 0) {
		printf("FAIL: sysv_msgget()\n"); exit(EXIT_FAILURE);
	}
	pr_out("Setup <SysV IPC:Message queue>");


	/* make epoll & */
	if ((epollfd = epoll_create(p_conninfo->max_open_files)) == -1) { /* error */
		exit(EXIT_FAILURE);
	}
	if ((ep_events = calloc(p_conninfo->max_open_files, sizeof(struct epoll_event))) == NULL) { /* error */
		exit(EXIT_FAILURE);
	}
	pr_out("Setup <Epoll> with %d max files", p_conninfo->max_open_files);

	return 0;
}

/* Name : make_threads
 * Desc : make threads of table
 * Argv : 
 * Ret  : zero(if success), negative(if fail)
 */
int make_threads()
{
	int		i, ret;

	for (i=0; i < MAX_THREAD_SLOT && pthr_slots[i].func != NULL; i++) {
		memset(pthr_slots[i].arg, 0, pthr_slots[i].sz_arg);
		if ((ret = pthread_create(&pthr_slots[i].pt_id, NULL, 
					pthr_slots[i].func, pthr_slots[i].arg)) < 0) { /* error */
			pr_out("[%d] Fail pthread_create() (%d:%s)", i, ret, strerror(ret));
		}
		printf("[%d] Create Thread\n", i);
	}

	/* make worker thread */
	for (i=0; i < MAX_WORKER_THREAD; i++) {
		Targ_worker[i].no = i;
		if ((ret = pthread_create((pthread_t *)&Targ_worker[i].pt_id, NULL, 
					start_worker, (void *)&Targ_worker[i])) < 0) { /* error */
			pr_out("[%d] Fail pthread_create() (%d:%s)", i, ret, strerror(ret));
		}
	}

	return 0;
}
