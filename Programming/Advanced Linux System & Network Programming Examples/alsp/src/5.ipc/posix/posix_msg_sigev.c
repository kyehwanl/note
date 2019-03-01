/* vim: set ts=4 sw=4: */
/* Filename    : posix_msg_sigev.c
 * Description : message queue with signal notification
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : usage) posix_msg sender filename , posix_msg receiver
 */
#define _XOPEN_SOURCE		600
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define NAME_POSIX_MQ	"/my_mq"
#define SZ_QUEUE		16384

mqd_t mq_fd;

int start_msq_sender(char *srcfile);
int start_msq_receiver();
void chk_rt(int sig, siginfo_t *si, void *data);

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage : %s <sender filename | receiver | unlink>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("* Message queue test program\n");
	if (argv[1][0] == 'u') { /* unlink MQ */
		mq_unlink(NAME_POSIX_MQ); exit(EXIT_SUCCESS);
	}

	/* open MQ. If you want to open MQ for nonblock mode, set oflag with O_NONBLOCK */
	if ((mq_fd = mq_open(NAME_POSIX_MQ, O_RDWR|O_CREAT|O_EXCL, 0660, NULL)) > 0) {
		printf("* Create MQ\n");
	} else {
		if (errno != EEXIST) { /* already exist shm */
			printf("FAIL: mq_open(): %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		mq_fd = mq_open(NAME_POSIX_MQ, O_RDWR);
		if (mq_fd == (mqd_t)-1) {
			printf("FAIL: mq_open(): %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* Send(Server) or Recv(Client) */
	switch(argv[1][0]) {
		case 's':
			printf("+ Sender start file(%s)\n", argv[2]);
			(void) start_msq_sender(argv[2]);
			printf("+ Finished. Press any key will exit.\n");
			getchar();
			break;
		case 'r':
			printf("+ Receiver start will waiting for message\n");
			(void) start_msq_receiver();
			break;
		default:
			fprintf(stderr, "* Unknown option, use sender or receiver\n");
			return 1; 
	}

	mq_close(mq_fd);

	return 0;
}

/* Name : start_msq_sender
 * Desc : read message data from file then send msg to MessageQueue
 * Argv : char *srcfile (src message file to read)
 * Ret  : 0 (if success), -1 (fail)
 */
int start_msq_sender(char *srcfile)
{
	FILE	*fp_srcfile;
	char	rbuf[SZ_QUEUE];
	int		len_rbuf;

	/* open source file */
	if ((fp_srcfile = fopen(srcfile, "r")) == NULL) {
		perror("FAIL: fopen()"); return -1;
	}

	while (!feof(fp_srcfile)) {
		if (fgets(rbuf, sizeof(rbuf), fp_srcfile) == NULL) { /* error or EOF */
			if (errno != 0) perror("Fail: fgets()"); 
			break; /* stop to read file */
		}

		/* There are no longger required last one byte bcoz CR */
		len_rbuf = strlen(rbuf) - 1;

		/* alternative func : mq_timedsend(), very usefull for blocking mode */
		printf("\t- Send (text:%.*s)\n", strlen(rbuf), rbuf);
		if (mq_send(mq_fd, rbuf, len_rbuf, 0) == -1) {
			perror("FAIL: mq_send()"); break;
		}
	}
	fclose(fp_srcfile); /* close file */

	return 0;
}

/* Name : start_msq_receiver
 * Desc : read message from MessageQueue
 * Argv : 
 * Ret  : 0 (if success), -1 (fail)
 */
int start_msq_receiver()
{
	int		n_recv;
	char	rbuf[SZ_QUEUE];
	struct sigevent	sigev_noti;
	struct sigaction sa_rt;
	struct mq_attr	mq_at;
	unsigned i = 0;

	/* install signal event handler */
	sa_rt.sa_sigaction = chk_rt;
	sigemptyset(&sa_rt.sa_mask);
	sa_rt.sa_flags = SA_SIGINFO|SA_RESTART;
	sigaction(SIGRTMIN, &sa_rt, NULL);

	/* setup notification */
	memset(&sigev_noti, 0, sizeof(struct sigevent));
	sigev_noti.sigev_notify = SIGEV_SIGNAL; /* set method of notification via signal delivery */
	sigev_noti.sigev_signo = SIGRTMIN;
	sigev_noti.sigev_value.sival_int = 100; /* transfer data */

	while (1) {
		i++;
		mq_getattr(mq_fd, &mq_at);
		printf("+ MQ status max/sz/cur = (%ld/%ld/%ld)\n", mq_at.mq_maxmsg, mq_at.mq_msgsize, mq_at.mq_curmsgs);
		if (mq_at.mq_curmsgs == 0) {
			/* install new notification event when end of message queue transaction */
			if (mq_notify(mq_fd, &sigev_noti) == -1) {
				if (errno != EBUSY) {
					perror("FAIL: mq_notify()"); return -1;
				}
			}
		}
		n_recv = mq_receive(mq_fd, rbuf, sizeof(rbuf), 0);
		if (n_recv == -1) {
			perror("FAIL: mq_receive()"); return -1;
		}
		printf("+ Recv(%.*s)\n", n_recv, rbuf);
	}
	return 0;
}

void chk_rt(int sig, siginfo_t *si, void *data)
{
	printf("[SIGRT] si_code(%d) si_band(%lx) si_fd(%d) si_value(%d)\n", 
			si->si_code, si->si_band, si->si_fd, si->si_value.sival_int);
}
