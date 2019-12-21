/* vim: set ts=4 sw=4: */
/* Filename    : aio_listio.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : Copy file with suffix ".copy" & use asynchronous I/O
                 Send RT signal when complete AIO with nonblocking mode.
 */
#include <signal.h>
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
#define SIGEV_RTSIG			SIGRTMIN+1

void chk_sigev_rt1(int signum, siginfo_t *si, void *sv);

int main(int argc, char *argv[])
{
	int		fd_rd, fd_wr;
	int		i =0, tot_len = 0, ret;
	char	ofname[0xff], rbuf[NUM_AIOCB_ARRAY][1024];
	struct aiocb	aio_blk[NUM_AIOCB_ARRAY];
	struct aiocb * const aiolist[] = { &aio_blk[0],&aio_blk[1],&aio_blk[2],&aio_blk[3],&aio_blk[4] };
	struct sigevent sigev = { .sigev_notify = SIGEV_NONE };
	struct sigaction sa_sigev;

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

	/* install signal handler */
	memset(&sa_sigev, 0, sizeof(struct sigaction));
	sa_sigev.sa_sigaction = chk_sigev_rt1;
	sigfillset(&sa_sigev.sa_mask);	/* blocking signal. So it can't be allowed interrupted by other signal */
	sa_sigev.sa_flags = SA_SIGINFO;
	sigaction(SIGEV_RTSIG, &sa_sigev, NULL);

	memset(aio_blk, 0, sizeof(struct aiocb) * NUM_AIOCB_ARRAY);
	while(1) {
		ret = read(fd_rd, rbuf[i], sizeof(rbuf[i]));
		if (ret == -1) { /* error */ }
		if (i==(NUM_AIOCB_ARRAY-1) || ret == 0) { /* the time to write */
			sigev.sigev_notify = SIGEV_SIGNAL; /* to be notified of AIO completion with thread */
			sigev.sigev_signo = SIGEV_RTSIG;
			sigev.sigev_value.sival_int = tot_len; /* value for siginfo_t */

			lio_listio(LIO_NOWAIT, aiolist, i+1, &sigev); /* LIO_NOWAIT mode shall be notified by sigevent */
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
	sleep(10);
	close(fd_rd); close(fd_wr);
	printf("> write complete\n");
	return EXIT_SUCCESS;
}

void chk_sigev_rt1(int signum, siginfo_t *si, void *sv)
{
	printf("\t (signo:%d) (band:%ld, si_value:%d)\n", si->si_signo, si->si_band, si->si_value.sival_int);
}
