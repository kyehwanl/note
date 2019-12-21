/* vim: set ts=4 sw=4: */
/* Filename    : in_dgram_broadcast_snd.c
 * Description : broadcasting UDP packet
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include "io_macro.h"

int main(int argc, char *argv[])
{
	struct sockaddr_in	saddr_s;
	int		udp_fd, sockopt, sz_slen;
	char	a_sbuf[50];

	/* init variables */
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));

	if (argc != 2) {
		printf("%s <port>", argv[0]);
		exit(EXIT_FAILURE);
	}

	udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (udp_fd == -1) {
		pr_err("[UDP broadcast] : Fail: socket()"); exit(EXIT_FAILURE);
	}
	sockopt = 1;
	if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt))) {
		pr_err("[UDP broadcast] : Fail: setsockopt()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_BROADCAST;
	saddr_s.sin_port = htons(((short)atoi(argv[1])));

	sprintf(a_sbuf, "%s", "UDP broadcasting test");
	pr_out("- Send broadcasting data every 3 sec");
	while(1) {
		sz_slen = sendto(udp_fd, a_sbuf, strlen(a_sbuf), 0, (struct sockaddr *) &saddr_s, sizeof(saddr_s));
		if (sz_slen == -1) {
			pr_err("[UDP broadcast] : Fail: sendto()");
		} else {
			printf("<< send broadcasting msg\n");
		}
		sleep(3);
	}
	return 0;
}

