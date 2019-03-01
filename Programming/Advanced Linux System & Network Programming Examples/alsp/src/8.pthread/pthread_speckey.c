/* vim: set ts=4 sw=4: */
/* Filename    : pthread_speckey.c
 * Description : condition variable & specified-data key
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_THREAD	5
#define MAX_Q_SIZE	20
#define PATH_FIFO	"/tmp/my_fifo"

pthread_key_t	pt_key_qdata;

void *tfunc_a(void *);			/* thread  (A) */
void *tfunc_b(void *);			/* thread  (B) */
struct thread_arg {
	pthread_t	pt_id;	/* thread id 	*/
	void *(* func)();
} thr_arg[MAX_THREAD] = {
	{0, tfunc_a},
	{0, tfunc_b},
	{0, tfunc_b},
	{0, tfunc_b},
	{0, NULL}
};

int process_job(int *);
void clean_thread(struct thread_arg *);
void bail_key_resource(void *mem);

struct share_data {
	int			queue[MAX_Q_SIZE];
	int			idx;		/* last index 	*/
	int			cnt;		/* item counter */
	pthread_mutex_t	mutex_data;
	pthread_cond_t	cv_data;
} *p_shr_data;

struct private_data {
	int		sum;
	int		last;
};

int main()
{
	int		i;
	struct timeval tv;

	/* init var, random seed */
	if ((p_shr_data = calloc(1, sizeof(struct share_data))) == NULL) {
		printf("[FAIL] calloc()\n"); exit(EXIT_FAILURE);
	};
	memset(p_shr_data, 0x00, sizeof(struct share_data));
	gettimeofday(&tv, NULL);
	srandom((unsigned int)tv.tv_usec);

	/* dynamically initialize mutex/cond/key */
	pthread_mutex_init(&p_shr_data->mutex_data, NULL);
	pthread_cond_init(&p_shr_data->cv_data, NULL);
	pthread_key_create(&pt_key_qdata, NULL);

	/* Start thread */
	for(i=0; i<MAX_THREAD && thr_arg[i].func != NULL; i++) {
		if (pthread_create(&thr_arg[i].pt_id, NULL, thr_arg[i].func, (void *)&thr_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create()\n");
			return 0;
		}
		printf("[MAIN] Create thread : tid = %lu\n", thr_arg[i].pt_id);
	}
	printf("[MAIN] Will join All threads!\n");
	clean_thread(thr_arg);
	return 0;
}

/* Name : tfunc_a
 * Desc : Thread (A)
 * Argv :
 * Ret  : None
 */
void *tfunc_a(void *arg)
{
	int		i, j, fd, n_read = 0;
	char	flag_empty;
	char	a_buf[0xff];

	printf("[ Thread (A) started! ]\n");

	/* make FIFO */
	if (mkfifo(PATH_FIFO, 0644) < 0) {
		if (errno != EEXIST) {
			fprintf(stdout, "[THREAD-A] FAIL: mkfifo()\n");
			exit(EXIT_FAILURE);
		}
	}
	if ((fd = open(PATH_FIFO, O_RDONLY, 0644)) < 0) {
		fprintf(stdout, "[THREAD-A] FAIL: open()\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		flag_empty = 0;

		if ( (n_read = read(fd, a_buf, sizeof(a_buf))) < 0) {
			fprintf(stdout, "[THREAD-A] FAIL: read()\n"); exit(EXIT_FAILURE); /* error */
		}
		if (n_read == 0) { /* close pipe */
			fprintf(stdout, "[THREAD-A] broken pipe\n"); exit(EXIT_FAILURE); 
		}

		if (pthread_mutex_lock(&p_shr_data->mutex_data)) { /* lock */
			fprintf(stdout, "[THREAD-A] FAIL: pthread_mutex_lock()\n");
			return NULL;
		}
		printf("[THREAD-A] Insert item to queue\n");

		/* i = buffer idx, j = shr_data->queue idx */
		if (p_shr_data->cnt == 0) flag_empty = 1;
		for (i=0, j=p_shr_data->idx; i<MAX_Q_SIZE && i<n_read; i++, j++) { 
			if (j == MAX_Q_SIZE) j=0;
			if (p_shr_data->cnt == MAX_Q_SIZE) {
				printf("- Queue full\n");
				break;
			}
			if (a_buf[i] == 0xa) break; /* skip new line */

			p_shr_data->queue[j] = (int) a_buf[i];
			printf("- Insert : queue[%d] : char(%c) -> int(%d)\n", j, a_buf[i], (int)a_buf[i]);
			p_shr_data->cnt++;
		}

		if (pthread_mutex_unlock(&p_shr_data->mutex_data)) { /* unlock */
			fprintf(stdout, "[THREAD-A] FAIL: pthread_mutex_lock()\n");
			return NULL;
		}

		if (flag_empty) { /* send signal */
			printf("[THREAD-A] Send signal to wake up thread\n");
			if (pthread_cond_broadcast(&p_shr_data->cv_data)) {
				fprintf(stdout, "[THREAD-A] FAIL: pthread_cond_signal()\n");
				return NULL;
			}
		}
	}
}

/* Name : tfunc_b
 * Desc : Thread (B)
 * Argv : 
 * Ret  : None
 */
void *tfunc_b(void *arg)
{
	int		buf_item;
	struct private_data	*p_pd;

	struct thread_arg *t_arg = arg;

	if ((p_pd = (struct private_data *)calloc(1, sizeof(struct private_data))) == NULL) {
		fprintf(stdout, "[THREAD-B] FAIL: calloc()\n");
		return NULL;
	}

	pthread_setspecific(pt_key_qdata, (void *)p_pd);

	printf("[ Thread (B) started! ]\n");

	while (1) {
		if (pthread_mutex_lock(&p_shr_data->mutex_data)) { /* lock */
			fprintf(stdout, "[THREAD-B] FAIL: pthread_mutex_lock()\n");
			return NULL;
		}

		if (p_shr_data->cnt == 0) {
			if (pthread_cond_wait(&p_shr_data->cv_data, &p_shr_data->mutex_data)) {
				fprintf(stdout, "[THREAD-B] FAIL: pthread_cond_wait()\n");
				return NULL;
			}
			printf("\t[THREAD-B:%lu] Wake up\n", t_arg->pt_id);
		}

		if (p_shr_data->cnt > 0) {
			printf("\t[Thread B:%lu] Pull item from queue\n", t_arg->pt_id);
			if (p_shr_data->idx == MAX_Q_SIZE) p_shr_data->idx = 0;
			buf_item = p_shr_data->queue[p_shr_data->idx++];
			p_shr_data->cnt--;
		}

		if (pthread_mutex_unlock(&p_shr_data->mutex_data)) { /* unlock */
			fprintf(stdout, "[THREAD-B] FAIL: pthread_mutex_lock()\n");
			return NULL;
		}

		process_job(&buf_item);
	}
}

/* Name : process_job
 * Desc : 
 * Argv : 
 * Ret  : None
 */
int process_job(int *item)
{
	struct private_data	*p_pd;
#define MODULAR_SECOND	5
	printf("\t- item = %d, will sleep (%d) sec.\n", *item, *item%MODULAR_SECOND);
	sleep(*item%MODULAR_SECOND);
	p_pd = (struct private_data *) pthread_getspecific(pt_key_qdata);
	p_pd->sum += *item;
	p_pd->last = *item;

	printf("[Thread:%lu] (%d/%d)\n", pthread_self(), p_pd->sum, p_pd->last);
	return 0;
}

/* Name : clean_thread
 * Desc : clean(join) thread
 * Argv : None
 * Ret  : None
 */
void clean_thread(struct thread_arg *t_arg)
{
	int			i;
	struct thread_arg *t_arg_ret;

	for (i=0; i<MAX_THREAD && t_arg->func != NULL; i++, t_arg++) {
		pthread_join(t_arg->pt_id, (void **)&t_arg_ret);
		printf("+ Thread id(%lu)\n", t_arg->pt_id);
	}
	printf("[Clean] All thread are joined.\n");
}
