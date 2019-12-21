/* vim: set ts=4 sw=4: */
/* Filename    : l_search.c
 * Description : linear search
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#define N_ARRAY			10
#define LEN_NAME		22
#define cp_string(A,B)	strncpy(A, B, strlen(B))
#define add_linear_data(s, arg_num, arg_name, arg_age)	memset(&s, 0, sizeof(s)); \
						s.num = arg_num; cp_string(s.name, arg_name); s.age = arg_age; \
		if (lsearch((void *)&s, p_larray, &n_larray, sizeof(s), comp_elem_data) == NULL) exit(EXIT_FAILURE);
typedef struct elem_data {
	int		num;
	char	name[LEN_NAME];
	short	age;
} ELEM_DATA;
int	n_elem;

void 	*p_larray;	/* base address of linear array */
size_t	n_larray;	/* number of array : will be modified by lsearch */

void print_elem(const ELEM_DATA *dest);
int add_elem(ELEM_DATA *dest, int num, const char *name, short age);
int comp_elem_data(const void *s1, const void *s2);

int main()
{
	ELEM_DATA	elem_data, *p_search_elem_data, key_elem_data;
	int		i;

	/* create and add data to linear array */
	printf("(*) Insert data : Array size = %d\n", N_ARRAY);
	if ( (p_larray = calloc(N_ARRAY, sizeof(ELEM_DATA))) == NULL) {
		/* erorr to allocate memory in heap */
		exit(EXIT_FAILURE);
	}

	/* input data */
	add_linear_data(elem_data, 1, "Paul", 42);
	add_linear_data(elem_data, 5, "Kate", 33);
	add_linear_data(elem_data, 11, "S. Kim", 29);
	add_linear_data(elem_data, 32, "T. Mikimoto", 17);
	add_linear_data(elem_data, 37, "Ben Smith", 22);
	add_linear_data(elem_data, 55, "Risa Anderson", 21);
	add_linear_data(elem_data, 27, "David Kim", 32);

	/* print data */
	for (i=0; i<n_larray; i++) {
		print_elem( &((ELEM_DATA *)p_larray)[i]);
	}

	printf("(*) Search for 'S. Kim' :\n");
	memset(&key_elem_data, 0, sizeof(ELEM_DATA));
	cp_string(key_elem_data.name, "S. Kim");	/* make key for searching */
	p_search_elem_data = (ELEM_DATA *) lfind(&key_elem_data, p_larray, &n_larray, sizeof(ELEM_DATA), comp_elem_data);
	if (p_search_elem_data != NULL) {
		print_elem(p_search_elem_data);
	} else {
		printf("\t[ Fail for search ]\n");
	}

	return 0;
}

void print_elem(const ELEM_DATA *dest)
{
	printf("\t[ %02d | %-*s | %02d ]\n", dest->num, sizeof(dest->name), dest->name, dest->age);
}

int add_elem(ELEM_DATA *dest, int num, const char *name, short age)
{
	memset(dest, 0, sizeof(ELEM_DATA));
	dest->num = num;
	strncpy(dest->name, name, LEN_NAME);
	dest->age = age;
	n_elem++;
	return 0;
}

int comp_elem_data(const void *s1, const void *s2)
{
	return (memcmp(((ELEM_DATA *)s1)->name, ((ELEM_DATA *)s2)->name, LEN_NAME));
}
