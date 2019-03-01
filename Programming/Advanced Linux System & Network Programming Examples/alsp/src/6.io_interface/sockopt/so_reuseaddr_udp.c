/* vim: set ts=4 sw=4: */
/* Filename    : so_reuseaddr_udp.c
 * Description : apply SO_REUSEADDR socket option for udp
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include "io_macro.h"
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

int main(int argc, char *argv[])
{
	struct sockaddr_in	saddr_s, saddr_c;
	int		serv_fd, sz_rlen, sz_slen;
	socklen_t len_saddr;
	short	port;
	char	a_rbuf[1024];
	int		sockopt = 1;

	/* init variables */
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));
	memset(&saddr_c, 0x00, sizeof(struct sockaddr_in));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc > 2) {
		printf("%s <port>", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	pr_out("[UDP server] : Process Start");
	serv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (serv_fd < 0) {
		pr_err("[UDP server] : Fail: socket()"); exit(EXIT_FAILURE);
	}

	if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
		pr_err("[UDP server] : Fail: setsockopt()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	saddr_s.sin_port = htons(port);

	if (bind(serv_fd, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) < 0) {
		pr_err("[UDP server] : Fail: bind()"); exit(EXIT_FAILURE);
	}

	if (port == 0) {
		len_saddr = sizeof(saddr_s);
		getsockname(serv_fd, (struct sockaddr *)&saddr_s, &len_saddr);
	}
	printf("[UDP server] : Port : #%d\n", ntohs(saddr_s.sin_port));

	while (1) {
		sz_rlen = recvfrom(serv_fd, a_rbuf, sizeof(a_rbuf), 0, (struct sockaddr *)&saddr_c, &len_saddr);
		if (sz_rlen < 0) {
			pr_err("[UDP server] : Fail: recvfrom()");
			printf("%s\n", strerror(errno));
		}
		printf("[recvfrom:%ld] (%d byte) (%.*s)\n", getpid(), sz_rlen, sz_rlen, a_rbuf);
		printf("[sendto  :%ld] client:(%s:%d)\n", getpid(), inet_ntoa(saddr_c.sin_addr),ntohs(saddr_c.sin_port));
		sz_slen = sendto(serv_fd, a_rbuf, sz_rlen, 0, (struct sockaddr *)&saddr_c, sizeof(saddr_c));
		if (sz_slen < 0) {
			pr_err("[UDP server] : Fail: sendto()");
		}
	}

	return 0;
}

