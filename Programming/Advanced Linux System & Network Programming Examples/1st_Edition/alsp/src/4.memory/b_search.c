/* vim: set ts=4 sw=4: */
/* Filename    : b_search.c
 * Description : binary search
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEN_NAME		22
#define cp_string(A,B)	strncpy(A, B, strlen(B))

typedef struct elem_data {
	int		num;
	char	name[LEN_NAME];
	short	age;
} ELEM_DATA;
int	n_elem;

void print_elem(const ELEM_DATA *dest);
int add_elem(ELEM_DATA *dest, int num, const char *name, short age);
int comp_elem_data(const void *s1, const void *s2);

int main()
{
	ELEM_DATA	*p_elem_data, *p_search_elem_data, key_elem_data;
	int			i;

	if ( (p_elem_data = (ELEM_DATA *) calloc(10, sizeof(ELEM_DATA))) == NULL) {
		/* error to get heap memory */
		exit(EXIT_FAILURE);
	}
	/* input data */
	add_elem(&p_elem_data[n_elem], 1, "Paul", 42);
	add_elem(&p_elem_data[n_elem], 5, "Kate", 33);
	add_elem(&p_elem_data[n_elem], 11, "S. Kim", 29);
	add_elem(&p_elem_data[n_elem], 32, "T. Mikimoto", 17);
	add_elem(&p_elem_data[n_elem], 37, "Ben Smith", 22);
	add_elem(&p_elem_data[n_elem], 55, "Risa Anderson", 21);
	add_elem(&p_elem_data[n_elem], 27, "David Kim", 32);

	printf("(*) Before qsort : \n");
	for (i=0; i<n_elem; i++) print_elem(&p_elem_data[i]);

	qsort(p_elem_data, n_elem, sizeof(ELEM_DATA), comp_elem_data);	/* Quick sort */

	printf("(*) After qsort : \n");
	for (i=0; i<n_elem; i++) print_elem(&p_elem_data[i]);

	printf("(*) Search for 'S. Kim' :\n");
	memset(&key_elem_data, 0, sizeof(ELEM_DATA));
	cp_string(key_elem_data.name, "S. Kim");	/* make key for searching */
	p_search_elem_data = (ELEM_DATA *) bsearch(&key_elem_data, 
			p_elem_data, n_elem, sizeof(ELEM_DATA), comp_elem_data);
	if (p_search_elem_data != NULL) {
		printf("-> %dth data :\n", p_search_elem_data - p_elem_data);
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
