/* vim: set ts=4 sw=4: */
/* Filename    : sock_hentname.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct hostent *p_hentlocal;
	int		i;
	char	a_hostname[100], *p_hostname;

	/* init variables */
	printf("Usage : %s <hostname>\n", argv[0]); 
	if (argc == 2) {
		p_hostname = argv[1];
	} else {
		/* default value with current hostname */
		if (gethostname(a_hostname, sizeof(a_hostname)) == -1) { /* error */ 
			return EXIT_FAILURE;
		}
		p_hostname = a_hostname;
	}

	if ((p_hentlocal = gethostbyname(p_hostname)) == NULL) { /* error */
		return EXIT_FAILURE;
	}

	printf("h_name : %s\n", p_hentlocal->h_name);
	printf("h_aliases: \n");
	for (i=0; p_hentlocal->h_aliases[i] != NULL; i++) {
		printf(">> %s\n", p_hentlocal->h_aliases[i]);
	}
	printf("h_addrtype : %d\n", p_hentlocal->h_addrtype);
	printf("h_addr_list: length(%d)\n", p_hentlocal->h_length);
	for (i=0; p_hentlocal->h_addr_list[i] != NULL; i++) {
		printf("IP : (string:%s) (hex:%lx)\n",
				inet_ntoa(*(struct in_addr *)p_hentlocal->h_addr_list[i]),
				ntohl(*(int *) p_hentlocal->h_addr_list[i]) );
	}

	return EXIT_SUCCESS;
}

