
#include <stdio.h>

char* hello(char* word)
{
  printf ("%s\n", word);

  return word;
}


/* compile option for shared library
 *
 *  gcc -o hello.so -shared -fPIC hello.c
 *
 */
