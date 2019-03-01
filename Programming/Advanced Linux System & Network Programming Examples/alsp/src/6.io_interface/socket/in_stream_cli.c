/* vim: set ts=4 sw=4: */
/* Filename    : in_stream_cli.c
 * Description : will send NORMAL band or OUT of band data over TCP
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

#define str_ordinary	"1.@@@@@"
#define str_OOB			"2.$$$$$"

int main(int argc, char *argv[])
{
	struct sockaddr_in	saddr_in;
	char	*p_sbuf;
	int		sz_slen;
	int		tcp_fd;
	int		i_choice, flag_send;
	char	a_input[10];

	memset(&saddr_in, 0x00, sizeof(struct sockaddr_in));

	/* if you want to use option list, rewrite below code with getopt() func. */
	if (argc != 3) {
		printf("%s <hostname> <port>\n", argv[0]); exit(EXIT_FAILURE);
	}

	tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (tcp_fd == -1) {
		pr_err("[Client] : Fail: socket()"); exit(EXIT_FAILURE);
	}

	/* setting socket address structure */
	saddr_in.sin_family = AF_INET;			/* AF_INET, PF_INET are same */
	if (inet_aton(argv[1], &saddr_in.sin_addr) == -1) {
		pr_err("[TCP client] : Fail: inet_aton()"); exit(EXIT_FAILURE);
	}
	saddr_in.sin_port = htons(((short)atoi(argv[2])));

	if (connect(tcp_fd, (struct sockaddr *)&saddr_in, sizeof(saddr_in)) == -1) {
		pr_err("[TCP client] : Fail: connect()"); exit(EXIT_FAILURE);
	}

#ifndef ENABLE_MSG_OOB
	pr_out("[TCP client] : 1:ordinary data  0:Exit program");
#else
	pr_out("[TCP client] : 1:ordinary data  2:OOB data  0:Exit program");
#endif
	while (1) {
		if (!fgets(a_input, sizeof(a_input), stdin)) continue;
		i_choice = atoi(a_input);

		switch(i_choice) {
			case 0:
				exit(EXIT_SUCCESS);
				break;

			case 1:
				p_sbuf = str_ordinary;
				flag_send = 0;
				printf(">> will send ordinary msg: data = [%s]\n", p_sbuf);
				break;

#ifdef ENABLE_MSG_OOB
			case 2:
				p_sbuf = str_OOB;
				flag_send = MSG_OOB;
				printf(">> will send OOB msg: data = [%s]\n", p_sbuf);
				break;
#endif
			default:
				printf(">> Error : Plz press 1 or 2, your input (%c:%d)\n", (char)i_choice+'0', i_choice);
				continue;
		}

		sz_slen = send(tcp_fd, p_sbuf, strlen(p_sbuf), flag_send);
		if (sz_slen == -1) {
			pr_err("[TCP client] : Fail: send()");
		}
	}

	return 0;
}

