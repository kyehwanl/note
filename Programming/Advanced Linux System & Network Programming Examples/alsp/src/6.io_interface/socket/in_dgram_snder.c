/* vim: set ts=4 sw=4: */
/* Filename    : in_dgram_snder.c
 * Description : send UDP packet over network
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

int
main(int argc, char *argv[])
{
	struct sockaddr_in	saddr_s, saddr_c;
	int		sz_rlen, sz_slen, tot_rlen = 0;
	socklen_t	len_saddr;
	char	a_sbuf[20], a_rbuf[16384];
	int		udp_fd, i;
	size_t	opt_sock;

	/* init variables */
	memset(&saddr_s, 0x00, sizeof(struct sockaddr_in));
	memset(&saddr_c, 0x00, sizeof(struct sockaddr_in));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc != 3) {
		printf("%s <hostname> <port>", argv[0]); exit(EXIT_FAILURE);
	}

	udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (udp_fd == -1) {
		pr_err("[Client] : Fail: socket()"); exit(EXIT_FAILURE);
	}
	opt_sock = 32768;	/* Linux will allocate buffer with double size */
	if (setsockopt(udp_fd, SOL_SOCKET, SO_RCVBUF, &opt_sock, sizeof(opt_sock))) {
		exit(EXIT_FAILURE); /* error */
	}

	/* setting socket address structure */
	saddr_s.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	if (inet_aton(argv[1], &saddr_s.sin_addr) == -1) {
		pr_err("[UDP Sender] : Fail: inet_aton()"); exit(EXIT_FAILURE);
	}
	saddr_s.sin_port = htons(((short)atoi(argv[2])));

	sprintf(a_sbuf, "%s", "UDP Sender");
	sz_slen = sendto(udp_fd, a_sbuf, strlen(a_sbuf), 0, (struct sockaddr *) &saddr_s, sizeof(saddr_s));
	if (sz_slen == -1) {
		pr_err("[UDP Sender] : Fail: sendto()");
	}
	pr_out("- Press any key. It will start retrieving data ... ");
	getchar();	/* just wait until user input ... */

	for (i=0; ;i++) {
		sz_rlen = recvfrom(udp_fd, a_rbuf, sizeof(a_rbuf), 0, (struct sockaddr *)&saddr_c, &len_saddr);
		if (sz_rlen == -1) {
			pr_err("[UDP Sender] : Fail: recvfrom()");
		}
		tot_rlen += sz_rlen;
		printf("[RECV:%d] client:(%s:%d) (tot_rlen = %d, current rlen = %d)\n", 
				i, (char *)inet_ntoa(saddr_c.sin_addr), (int)ntohs(saddr_c.sin_port), tot_rlen, sz_rlen);
	}

	return 0;
}

