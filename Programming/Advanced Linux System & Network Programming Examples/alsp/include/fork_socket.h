/* vim: set ts=4 sw=4 : */
#ifndef FORK_SOCKET_H
#define FORK_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define LISTEN_BACKLOG	20
#define MSG_SORRY	"Occur error, just close session"
#define USAGE	"Usage) %s [port]\n"\
				"\t[port] : listening port\n"\
				"\t       : if dosen't specified port no, allocate random port\n"


/* Process table for multiple process manager */
typedef struct _PROCESS_TAB {
	int		id_no;
	int		fd;
	time_t	login_time;
} PROCESS_TAB;

extern PROCESS_TAB	*process_tab;

#endif
