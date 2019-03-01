/* vim: set ts=4 sw=4: */
/* Filename    : padding.c
 * Description : struct padding
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stddef.h>

#define pr_struct(postfix_name)	printf("sizeof(MY_ST_"#postfix_name") = %d, offsetof(...,cnt) = 0x%04x\n", \
		sizeof(MY_ST_##postfix_name), offsetof( MY_ST_##postfix_name,cnt))

typedef struct my_st_a {
	char	str[9];
	char	cnt[4];
} MY_ST_A;

typedef struct my_st_b {
	char	str[9];
	int 	cnt;
} MY_ST_B;

typedef struct my_st_c {
	char	str[9];
	short	cnt;
} MY_ST_C;

typedef struct my_st_d {
	char	str[9];
	double	cnt;
} MY_ST_D;

int main()
{
	pr_struct(A);
	pr_struct(B);
	pr_struct(C);
	pr_struct(D);
	return 0;
}
