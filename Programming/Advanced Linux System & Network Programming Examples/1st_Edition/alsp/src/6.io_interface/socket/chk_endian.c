/* vim: set ts=4 sw=4: */
/* Filename    : chk_endian.c
 * Description : view byte order
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <netinet/in.h>

union byte_long {
	long	l;
	unsigned char	c[4];
};

int main()
{
	union byte_long		bl;

	bl.l = -1200000L;
	printf("(%02x-%02x-%02x-%02x)\n", bl.c[0], bl.c[1], bl.c[2], bl.c[3]);
	bl.l = htonl(bl.l);
	printf("(%02x-%02x-%02x-%02x)\n", bl.c[0], bl.c[1], bl.c[2], bl.c[3]);

	return 0;
}
