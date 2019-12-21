/* vim: set ts=4 sw=4: */
/* Filename    : un_dgram.c
 * Description : UNIX domain socket with UDP
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <arpa/inet.h>
#include "io_macro.h"

#define PATH_UNIX_SOCKET	"/tmp/my_socket"

int main(int argc, char *argv[])
{
	struct sockaddr_un	saddru_rv;
	int		ufd, sz_rlen;
	socklen_t	len_saddr;
	char	a_buf[1024];
	int		flag_recver;	/* 0:receiver, otherwise:sender */

	/* init variables */
	memset(&saddru_rv, 0x00, sizeof(struct sockaddr_un));
	len_saddr = sizeof(struct sockaddr_un);

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc != 2) {
		printf("%s <receiver | sender>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argv[1][0] == 'r') 
		flag_recver = 1;
	else 
		flag_recver = 0;

	ufd = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_IP);
	if (ufd == -1) {
		pr_err("[UNIX_Socket] : Fail: socket()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddru_rv.sun_family = AF_UNIX;			/* AF_UNIX, PF_UNIX are same */
	sprintf(saddru_rv.sun_path, PATH_UNIX_SOCKET);

	printf("[sender] : socket path: %s\n", PATH_UNIX_SOCKET);
	if (flag_recver) { /* receiver */
		/* remove exist socket */
		if (remove(PATH_UNIX_SOCKET)) {
			if (errno != ENOENT) { /* exit when occur error except "No such file.." */
				pr_err("[UNIX Socket] : Fail: remove()"); exit(EXIT_FAILURE);
			}
		}

		if (bind(ufd, (struct sockaddr *)&saddru_rv, sizeof(saddru_rv)) == -1) {
			pr_err("[UNIX Socket] : Fail: bind()"); exit(EXIT_FAILURE);
		}
		while (1) {
#ifndef USE_OTHER_FUNCTION
			sz_rlen = recvfrom(ufd, a_buf, sizeof(a_buf), 0, 
					NULL, NULL);
			if (sz_rlen == -1) {
				pr_err("[UDP Socket] : Fail: recvfrom()");
			}
#else
			sz_rlen = read(ufd, a_buf, sizeof(a_buf));
			if (sz_rlen == -1) {
				pr_err("[UDP Socket] : Fail: read()");
			}
#endif
			printf("[recv:%d] (%d byte) (%.*s)\n", getpid(), sz_rlen, sz_rlen, a_buf);
		}
	} else { /* sender */
#ifdef USE_OTHER_FUNCTION
		if (connect(ufd, (struct sockaddr *)&saddru_rv, len_saddr) == -1) {
			pr_err("[UNIX Socket] : Fail: connect()"); exit(EXIT_FAILURE);
		}
#endif
		while (1) {
			if (fgets(a_buf, sizeof(a_buf), stdin) == NULL) {
				/* error */
			}
#ifndef USE_OTHER_FUNCTION
			sz_rlen = sendto(ufd, a_buf, strlen(a_buf)-1,
					0, (struct sockaddr *)&saddru_rv, len_saddr);
			if (sz_rlen == -1) {
				pr_err("[UDP Socket] : Fail: sendto()");
			}
#else
			sz_rlen = write(ufd, a_buf, strlen(a_buf)-1);
			if (sz_rlen == -1) {
				pr_err("[UDP Socket] : Fail: write()");
			}
#endif
			printf("[send:%d] (%d byte) (%.*s)\n", getpid(), sz_rlen, sz_rlen, a_buf);
		}
	}
	return 0;
}

