/* vim: set ts=4 sw=4: */
/* Filename    : sysv_msg_info.c
 * Description : message queue test code
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : usage) sysv_msg sender filename , sysv_msg receiver 3
 */
#define _GNU_SOURCE
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void print_mq_ds(struct msqid_ds *ds);
void print_mq_info(struct msginfo *info);

int main(int argc, char *argv[])
{
	int		msg_id;
	struct msqid_ds	mq_ds;	/* MSG-Q Data structure */
	struct msginfo	mq_info;/* MSG-Q Info structure */
	if (argc != 2) {
		printf("Usage : %s <MSG-Q IPC ID>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("* MSG-Q Info with IPC ID:%s (Ctrl-C:break, Enter:Print)\n", argv[1]);

	msg_id = atoi(argv[1]);
	if (msg_id < 0) {
		printf("FAIL: msg_init()\n");
		exit(EXIT_FAILURE);
	}

	/* Send(Server) or Recv(Client) */
	while(1) {
		if ((msgctl(msg_id, IPC_STAT, &mq_ds) |
					msgctl(msg_id, IPC_INFO, (struct msqid_ds *)&mq_info)) == -1) {
			perror("FAIL: msgctl()"); 
			exit(EXIT_FAILURE);
		}
		print_mq_ds(&mq_ds);
		print_mq_info(&mq_info);
		getchar();
	}
	return 0;
}

/* Name : print_mq_ds
 * Desc : print message queue data structure
 * Argv : struct msgid_ds *ds
 * Ret  : None
 */
void print_mq_ds(struct msqid_ds *ds)
{
	printf("Byte Current/Max(%lu/%lu), # of Message(%lu)\n",
			ds->msg_cbytes, ds->msg_qbytes, ds->msg_qnum);
}

/* Name : print_mq_info
 * Desc : print message queue info structure
 * Argv : struct msginfo *info
 * Ret  : None
 */
void print_mq_info(struct msginfo *info)
{
	printf("msgssz(%d) msgseg(%d) msgmax(%d) msgmni(%d) msgmnb(%d)\n",
			info->msgssz, info->msgseg, info->msgmax, info->msgmni, info->msgmnb);

}
