/* vim: set ts=4 sw=4: */
/* Filename    : t_search.c
 * Description : T tree searching
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#define LEN_NAME		22
#define cp_string(A,B)	strncpy(A, B, strlen(B))

typedef struct elem_data {
	int		num;
	char	name[LEN_NAME];
	short	age;
} ELEM_DATA;
int	n_elem;

void *p_tree;	/* root tree : bss will be initializing to NULL(0). */

void print_elem(const ELEM_DATA *dest);
int add_elem(ELEM_DATA *dest, int num, const char *name, short age);
int comp_elem_data(const void *s1, const void *s2);
void walk_print(const void *nodep, const VISIT which, const int depth);

int main()
{
	ELEM_DATA	*p_elem_data, tmp_elem_data = {.num = 0, .age = 0};
	void 		*p_res_tree;	/* action result of tree function */
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
	printf("(*) Current data :\n");
	for (i=0; i<n_elem; i++) print_elem(&p_elem_data[i]);

	/* create and add data to tree */
	for (i=0; i<n_elem; i++) {
		if ((p_res_tree = tsearch((void *)&p_elem_data[i], &p_tree, comp_elem_data)) == NULL) {
			/* error */
			exit(EXIT_FAILURE);
		}
	}
	printf("(*) Walking into tree :\n");
	twalk(p_tree, walk_print);

	printf("(*) Search for 'S. Kim' :\n");
	memset(&tmp_elem_data, 0, sizeof(tmp_elem_data));
	cp_string(tmp_elem_data.name, "S. Kim");

	p_res_tree = tfind(&tmp_elem_data, &p_tree, comp_elem_data);
	if (p_res_tree != NULL) {
		print_elem(*(ELEM_DATA **)p_res_tree);
	} else {
		printf("\t[ Fail for search ]\n");
	}

	printf("(*) Delete for 'David Kim' :\n");
	memset(&tmp_elem_data, 0, sizeof(tmp_elem_data));
	cp_string(tmp_elem_data.name, "David Kim");
	if ((p_res_tree = tdelete(&tmp_elem_data, &p_tree, comp_elem_data)) == NULL) {
		printf("\t[ Can't find key about '%s' ]\n", tmp_elem_data.name);
		exit(EXIT_FAILURE);
	}
	printf("(*) After delete ...\n");
	twalk(p_tree, walk_print);

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

void walk_print(const void *nodep, const VISIT which, const int depth)
{
	ELEM_DATA	*p_elem;

	switch(which) {
		case preorder:
			break;
		case postorder:
			p_elem = *(ELEM_DATA **)nodep;
			print_elem(p_elem);
			break;
		case endorder:
			break;
		case leaf:
			p_elem = *(ELEM_DATA **)nodep;
			print_elem(p_elem);
			break;
	}
}
