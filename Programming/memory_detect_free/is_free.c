#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


/* http://stackoverflow.com/questions/8300853/how-to-check-if-a-pointer-is-freed-already-in-c */
/* IT'S NOT WORKING !!! ABSOLUTELY GABAGE */
int is_freed(void *p)
{
    void * q;
    char p_addr [50];
    char q_addr [50];

    sprintf(p_addr, "%p", p);

    q = malloc (1);
    sprintf(q_addr, "%p", q);
    free (q);

    return ! strcmp(q_addr, p_addr);
}



void main(void)
{
  void* p1= malloc(10);


  printf("1st try: %d \n", is_freed(p1));

  free(p1);

  void* p2= malloc(20);
  void* p3= malloc(10);

  free(p2);
  free(p3);

  p3= malloc(10);

  printf("2nd try: %d \n", is_freed(p1));
  printf("3nd try: %d \n", is_freed(p2));

  printf("4th: %d \n", malloc_usable_size(p1));
  printf("5th: %d \n", malloc_usable_size(p2));
  printf("6th: %d \n", malloc_usable_size(p3));

}
