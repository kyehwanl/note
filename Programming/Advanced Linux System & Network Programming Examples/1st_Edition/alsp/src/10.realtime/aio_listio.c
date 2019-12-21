/* vim: set ts=4 sw=4: */
/* Filename    : aio_listio.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : Copy file with suffix ".copy" & use asynchronous I/O
                 Create thread when complete listed AIO with nonblocking mode.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <aio.h>
#include <errno.h>
#define NUM_AIOCB_ARRAY		5

#ifdef ASYNCHRONIZED_IO
#include <pthread.h>
void start_sigev_aio(sigval_t	arg);
pthread_attr_t	pt_attr;
#endif

int main(int argc, char *argv[])
{
	int		fd_rd, fd_wr;
	int		i =0, tot_len = 0, ret;
	char	ofname[0xff], rbuf[NUM_AIOCB_ARRAY][1024 * 256];
	struct aiocb	aio_blk[NUM_AIOCB_ARRAY];
	struct aiocb * const aiolist[] = { &aio_blk[0],&aio_blk[1],&aio_blk[2],&aio_blk[3],&aio_blk[4] };
#ifdef ASYNCHRONIZED_IO
	struct sigevent sigev = { .sigev_notify = SIGEV_NONE };
#endif

	if (argc != 2) {
		printf("Usage : %s <source file>\n", argv[0]); return EXIT_SUCCESS;
	}
	sprintf(ofname, "%s.copy", argv[1]);
	printf("file copy : %s -> %s\n", argv[1], ofname);
	if ((fd_rd = open(argv[1], O_RDONLY, 0)) == -1) {
		perror("Fail: open()"); exit(EXIT_FAILURE);
	}
	if ((fd_wr = open(ofname, O_CREAT|O_WRONLY, 0644)) == -1) {
		perror("Fail: open()"); exit(EXIT_FAILURE);
	}

	memset(aio_blk, 0, sizeof(struct aiocb) * NUM_AIOCB_ARRAY);
	while(1) {
		ret = read(fd_rd, rbuf[i], sizeof(rbuf[i]));
		if (ret == -1) { /* error */ }
		if (i==(NUM_AIOCB_ARRAY-1) || ret == 0) { /* the time to write */
#ifdef ASYNCHRONIZED_IO
			sigev.sigev_notify = SIGEV_THREAD; /* to be notified of AIO completion with thread */
			sigev.sigev_value.sival_int = tot_len; /* thread argument */

			pthread_attr_init(&pt_attr); /* thread attribute */
			pthread_attr_setdetachstate(&pt_attr, PTHREAD_CREATE_DETACHED); /* set detached */
			sigev.sigev_notify_attributes = &pt_attr;
			sigev.sigev_notify_function = start_sigev_aio; /* thread start function */
			//lio_listio(LIO_NOWAIT, aiolist, i+1, &sigev); /* LIO_NOWAIT mode shall be notified by sigevent */
			lio_listio(LIO_NOWAIT, aiolist, i+1, NULL); /* LIO_NOWAIT mode shall be notified by sigevent */
#else
			lio_listio(LIO_WAIT, aiolist, i+1, NULL); /* synchronized I/O */
#endif
			memset(aio_blk, 0, sizeof(struct aiocb) * NUM_AIOCB_ARRAY);
			i=0; /* reset index */
		}
		aio_blk[i].aio_fildes = fd_wr; 	/* operated file descript 	*/
		aio_blk[i].aio_buf = rbuf[i]; 	/* buffer to write */
		aio_blk[i].aio_nbytes = ret;	/* number of bytes */
		aio_blk[i].aio_offset = tot_len;/* start offset to write */
		aio_blk[i].aio_lio_opcode = LIO_WRITE; /* operation mode */

		/* update conditions */
		tot_len += ret;
		i++;
		if (ret == 0) break; /* condition to exit loop: means get EOF from file */
	}
#ifdef ASYNCHRONIZED_IO
	sleep(10);  /* killing time to wait for notification.
				   but, there are some problem when delayed thread invokation over ten seconds.
				   then, we need more strict mechanism for notification. it's homework.
				 */
#endif
	printf("> write complete\n");
	close(fd_rd); close(fd_wr);
	return (EXIT_SUCCESS);
}

#ifdef ASYNCHRONIZED_IO
void start_sigev_aio(sigval_t arg)
{
	printf("\t[Notification via thread] AIO copy %d byte.\n", arg.sival_int);
}
#endif
