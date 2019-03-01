/* vim: set ts=4 sw=4: */
/* Filename    : h_search.c
 * Description : searching with hash table
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

void print_elem(const ELEM_DATA *dest);
int add_elem(ELEM_DATA *dest, int num, const char *name, short age);

int main()
{
	ELEM_DATA	*p_elem_data;
	ENTRY		hentry, *p_hentry;
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

	/* create and add data to hash */
	hcreate(30);
	for (i=0; i<n_elem; i++) {
		hentry.key = p_elem_data[i].name;
		hentry.data = &p_elem_data[i];
		hsearch(hentry, ENTER);	/* insert data to hash table */
	}

	printf("(*) Search for 'S. Kim' :\n");
	hentry.key = "S. Kim";
	p_hentry = hsearch(hentry, FIND);
	if (p_hentry != NULL) {
		print_elem(p_hentry->data);
	} else {
		printf("\t[ Fail for search ]\n");
	}
	hdestroy();

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
