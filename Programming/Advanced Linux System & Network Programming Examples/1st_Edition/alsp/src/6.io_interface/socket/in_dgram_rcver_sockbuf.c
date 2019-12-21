/* vim: set ts=4 sw=4: */
/* Filename    : in_dgram_rcver_sockbuf.c
 * Description : UDP socket with specified size of socket buffer
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : when lost udp packet on LAN, you might be more reliable with larger socket buffer
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

int
main(int argc, char *argv[])
{
	struct sockaddr_in	saddr_s, saddr_c;
	int		serv_fd, sz_rlen, sz_slen;
	socklen_t len_saddr;
	short	port;
	char	a_rbuf[1024], a_sbuf[16384];
	int		opt_sock, i;

	/* init variables */
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));
	memset(&saddr_c, 0x00, sizeof(struct sockaddr_in));
	memset(a_sbuf, '#', sizeof(a_sbuf));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc > 2) {
		printf("%s <port>", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		port = (short) atoi((char *)argv[1]);
	} else {
		port = 0;	/* random port */
	}

	serv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (serv_fd == -1) {
		pr_err("[UDP Receiver] : Fail: socket()"); exit(EXIT_FAILURE);
	}
	opt_sock = 8192;	/* Is it fail when socket buffer smaller than data size? */
	if (setsockopt(serv_fd, SOL_SOCKET, SO_SNDBUF, &opt_sock, sizeof(opt_sock))) {
		pr_err("[UDP Receiver] : Fail: setsockopt()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	saddr_s.sin_addr.s_addr = INADDR_ANY;	/* localhost */
	saddr_s.sin_port = htons(port);

	if (bind(serv_fd, (struct sockaddr *)&saddr_s, sizeof(saddr_s)) == -1) {
		pr_err("[UDP Receiver] : Fail: bind()"); exit(EXIT_FAILURE);
	}

	if (port == 0) {
		len_saddr = sizeof(saddr_s);
		getsockname(serv_fd, (struct sockaddr *)&saddr_s, &len_saddr);
	}
	printf("[UDP Receiver] : Port : #%d\n", ntohs(saddr_s.sin_port));

	while (1) {
		sz_rlen = recvfrom(serv_fd, a_rbuf, sizeof(a_rbuf), 0, (struct sockaddr *)&saddr_c, &len_saddr);
		if (sz_rlen == -1) { /* error */
			pr_err("[UDP Receiver] : Fail: recvfrom()"); 
		}
		printf("[recvfrom] (%d byte) (%.*s)\n", sz_rlen, sz_rlen, a_rbuf);
		for(i=0;i<10;i++) { /* send 160 KiB */
			sz_slen = sendto(serv_fd, a_sbuf, sizeof(a_sbuf), 0, (struct sockaddr *)&saddr_c, sizeof(saddr_c));
			if (sz_slen == -1) { /* error */
				pr_err("[UDP Receiver] : Fail: sendto()");	
			}
			printf("[%d] send (%d) byte\n", i, sz_slen);
		}
		printf("[sendto  ] client:(%s:%d) send (%d) byte. complete\n", 
				inet_ntoa(saddr_c.sin_addr), ntohs(saddr_c.sin_port), sizeof(a_sbuf) * 10);
	}

	return 0;
}

