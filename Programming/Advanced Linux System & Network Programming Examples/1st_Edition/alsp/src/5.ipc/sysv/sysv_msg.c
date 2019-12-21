/* vim: set ts=4 sw=4: */
/* Filename    : sysv_msg.c
 * Description : message queue test code
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : usage) sysv_msg sender filename , sysv_msg receiver 3
 */
#include "sysv_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SZ_QUEUE	16384

int msg_id;
struct mq_buf {
	long mtype;
	char mtext[SZ_QUEUE];
};

int start_msq_sender(char *srcfile);
int start_msq_receiver(long mtype);

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage : %s <sender | receiver> <filename or mtype>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("* Message queue test program\n");

	msg_id = sysv_msgget(argv[0], 0, 0664);
	if (msg_id < 0) {
		printf("FAIL: msg_init()\n");
		exit(EXIT_FAILURE);
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
			printf("+ Receiver start with type(%c)\n", argv[2][0]);
			(void) start_msq_receiver((long) (argv[2][0] - '0'));
			break;
		default:
			fprintf(stderr, "* Unknown option, use sender or receiver\n");
			return 1; 
	}

	sysv_msgrm(msg_id);

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
	struct mq_buf mq_buf;
	int		len_mtext;

	/* open source file */
	if ((fp_srcfile = fopen(srcfile, "r")) == NULL) {
		perror("FAIL: fopen()"); return -1;
	}

	while (!feof(fp_srcfile)) {
		if (fgets(rbuf, sizeof(rbuf), fp_srcfile) == NULL) { /* error or EOF */
			if (errno != 0) perror("Fail: fgets()"); 
			break; /* stop to read file */
		}

		/* read message from file, then extract mtype and mtext */
		mq_buf.mtype = (long) (rbuf[0] - '0');
		len_mtext = strlen(rbuf) - 3;
		memcpy(mq_buf.mtext, (char *)&rbuf[2], len_mtext);
		printf("\t- Send (mtype:%ld, mtext:%.*s)\n", mq_buf.mtype, len_mtext, mq_buf.mtext);
		if (msgsnd(msg_id, (struct msgbuf *)&mq_buf, len_mtext, IPC_NOWAIT) == -1) {
			perror("FAIL: msgsnd()"); break;
		}
	}
	fclose(fp_srcfile); /* close file */

	return 0;
}

/* Name : start_msq_receiver
 * Desc : read message from MessageQueue
 * Argv : long mtype (message type for reading)
 * Ret  : 0 (if success), -1 (fail)
 */
int start_msq_receiver(long mtype)
{
	int		n_recv;
	struct mq_buf mq_buf;

	while (1) {
		n_recv = msgrcv(msg_id, (struct msgbuf *)&mq_buf, SZ_QUEUE, mtype, 0);
		if (n_recv == -1) {
			perror("FAIL: msgrcv()"); break;
		}
		printf("+ Recv(%ld)-(%.*s)\n", mtype, n_recv, mq_buf.mtext);
	}
	return 0;
}

