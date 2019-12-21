/* vim: set ts=4 sw=4: */
/* Filename    : posix_msg.c
 * Description : message queue test code
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : usage) posix_msg sender filename , posix_msg receiver
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>
#include "io_macro.h"

#define NAME_POSIX_MQ	"/my_mq"
#define SZ_QUEUE		16384

mqd_t mq_fd;

int start_msq_sender(char *srcfile);
int start_msq_receiver();

int main(int argc, char *argv[])
{
	/* checking system limits concerned with POSIX MQ */
	struct mq_attr	mq_attrib = {.mq_flags = 0, .mq_maxmsg = 20, .mq_msgsize = 4096 };
	if (argc < 2) {
		printf("Usage : %s <sender filename | receiver | unlink>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("* Message queue test program\n");
	if (argv[1][0] == 'u') { /* unlink MQ */
		mq_unlink(NAME_POSIX_MQ); exit(EXIT_SUCCESS);
	}

	/* open MQ. If you want to open MQ for nonblock mode, set oflag with O_NONBLOCK */
	if ((mq_fd = mq_open(NAME_POSIX_MQ, O_RDWR|O_CREAT|O_EXCL, 0660, &mq_attrib)) > 0) {
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

	while (1) {
		n_recv = mq_receive(mq_fd, rbuf, sizeof(rbuf), 0);
		if (n_recv == -1) {
			perror("FAIL: mq_receive()"); break;
		}
		printf("+ Recv(%.*s)\n", n_recv, rbuf);
	}
	return 0;
}

