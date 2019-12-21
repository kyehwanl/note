#ifndef IO_EPOLL_THREAD_H
#define IO_EPOLL_THREAD_H

#include <pthread.h>
/* --------------------------------------------------------------------------*/
/* Resource Definitions                                                      */
/* --------------------------------------------------------------------------*/
#define DFL_OPEN_MAX_FILES		1024
#define MAX_WORKER_THREAD		4

/* epoll resources */
extern int		epollfd;	/* epoll fd	*/
extern struct epoll_event  *ep_events;

/* SysV IPC: Message queue */
#define MY_MSGKEY		0x123000
extern int		msg_id;		/* IPC id for message queue */
struct mq_buf {
	long	mtype;
	int		mtext[1];
};
enum {
	MQ_INTERNAL = 16,
	MQ_EXTERNAL
};
typedef struct mq_buf	MQ_BUF;

/* Connection information */
struct conninfo {
	int		cur_open_files;
	int		max_open_files;
	pthread_mutex_t		mutex_lock;
	pthread_cond_t		cv_barrier;
};
typedef struct conninfo CONNINFO;
extern CONNINFO	*p_conninfo;

/* fd list */
#define MAX_CLIENT_BUF	16384
#define N_FDLIST		1024
struct fdlist {
	int		fd;			/* file descriptor 					*/
	time_t	t_conn;		/* connected time					*/
	char	rbuf[MAX_CLIENT_BUF];
};
typedef struct fdlist FDLIST;
extern FDLIST	*p_fdlists;	/* client slot size */

/* Event list */
#define MAX_EVENT_QUEUE	200
struct evlist {
	int		i1_ev;		/* start index of I/O queue			*/
	int		i2_ev;		/* end index of I/O queue			*/
	int		n_ev;		/* number of events in I/O queue	*/
	int		q_ev[MAX_EVENT_QUEUE];	/* event list queue 	*/
	pthread_mutex_t		mutex_lock;
	pthread_cond_t		cv_barrier;
};
typedef struct evlist EVLIST;
extern EVLIST	*p_evlists[MAX_WORKER_THREAD];

#if defined(WITH_RETRANS_POLLER)
#define MAX_RETRANS_QUEUE	512
struct retrans {
	time_t	to;			/* timeout 			*/
	int		fd;			/* fd to re-trans 	*/
	char	*s;			/* buffer to send	*/
};
typedef struct retrans RETRANS;

struct rtlist {			/* retransfer list 	*/
	short	n_rt;		/* last index 		*/
	RETRANS	rt[DFL_OPEN_MAX_FILES];	/* transaction */
	pthread_mutex_t		mutex_lock;
	pthread_cond_t		cv_barrier;
};
typedef struct rtlist RTLIST;
extern RTLIST	*p_rtlists;
#endif

/* --------------------------------------------------------------------------*/
/* Macro definitions                                                         */
/* --------------------------------------------------------------------------*/
#define ADD_FDLIST(fd_num)	(p_fdlists[fd_num]).fd = fd_num; \
							(p_fdlists[fd_num]).t_conn = time(NULL)
#define DEL_FDLIST(fd_num)	(p_fdlists[fd_num]).fd = -1
#define ADD_EFD(a, b)   	if (add_efd(a, b) < 0) { pr_err("Fail: add_efd"); exit(EXIT_FAILURE); }
#define DEL_EFD(a, b)   	if (del_efd(a, b) < 0) { pr_err("Fail: del_efd"); exit(EXIT_FAILURE); }

#define MUTEX_LOCK(t)       pthread_mutex_lock(&t->mutex_lock)
#define MUTEX_UNLOCK(t)     pthread_mutex_unlock(&t->mutex_lock)
#define CV_SIGNAL(t)        pthread_cond_broadcast(&t->cv_barrier)
#define CV_WAIT(t)          pthread_cond_wait(&t->cv_barrier, &t->mutex_lock)


/* --------------------------------------------------------------------------*/
/* Thread information                                                        */
/* --------------------------------------------------------------------------*/
typedef struct {
	pthread_t	pt_id;			/* pthread id				*/
	void *(*func)();			/* thread start func    	*/
	void *arg;					/* thraed argument			*/
	size_t	sz_arg;				/* size of arg member		*/
	char	*desc;				/* description				*/
} PTHR_SLOT;

/* --------------------------------------------------------------------------*/
/* Thread arguements                                                         */
/* --------------------------------------------------------------------------*/
enum { 					/* thread status */
	T_IDLE = 0,
	T_RUNNING
};

struct targ_listener { 		/* listener thread argument 	*/
	struct sockaddr_in	saddr_s;
	int		fd_listener;
};
extern volatile struct targ_listener Targ_listener;

struct targ_poller { 		/* poller thread argument 		*/
	unsigned int	n_epevent;
};
extern volatile struct targ_poller Targ_poller;

struct targ_iosched { 		/* io scheduler thread argument */
	int		n_worker;		/* number of worker thread 		*/
};
extern volatile struct targ_iosched Targ_iosched;

#define MAX_WORK_QUEUE	50
struct targ_worker { 	/* worker thread argument 		*/
	pthread_t	pt_id;	/* Pthread ID					*/
	int		status;		/* T_UNUSED, T_IDLE, T_RUNNING 	*/
	int		no;			/* sequence number				*/
};
extern volatile struct targ_worker Targ_worker[MAX_WORKER_THREAD];

#if defined(WITH_RETRANS_POLLER)
struct targ_sendq { 	/* sendQ thread argument 		*/
	pthread_t	pt_id;	/* Pthread ID					*/
	int		status;		/* T_UNUSED, T_IDLE, T_RUNNING 	*/
	int		no;			/* sequence number				*/
	int		n_retrans;	/* number of retransations		*/
	int		n_failed;	/* failed 						*/
};
extern volatile struct targ_sendq Targ_sendq;

#endif

/* -------------------------------------------------------------------------- */
/* Thread start functions                                                     */
/* -------------------------------------------------------------------------- */
extern void *start_listener(void *t_arg);  /* listener thread  */
extern void *start_poller(void *t_arg);    /* poller thread    */
extern void *start_iosched(void *t_arg);   /* I/O scheduler    */
extern void *start_worker(void *t_arg);    /* worker thread    */
extern void *start_sendq(void *t_arg);     /* sendq thread     */
/* -------------------------------------------------------------------------- */
/* I/O manipulation                                                           */
/* -------------------------------------------------------------------------- */
extern int add_efd(int efd, int fd);
extern int del_efd(int efd, int fd);
extern int push_evlist(EVLIST *evl, int fd);
extern int pop_evlist(EVLIST *evl);
extern inline FDLIST * get_fdlistbyfd(int fd);

#endif
