#define DIGITS 16
#define BASE 2
#include <stdio.h>
void convertBaseVersionOne(char input, char *output)
{
   int i, remainder;
  /*(With hex, you need the rest of these characters, not just 0 and 1)*/
   char digitsArray[17] = "0123456789ABCDEF";


   for (i = DIGITS; i > 0; i--)
   {
      remainder = input % BASE;
      input = input / BASE;
      output[i - 1] = digitsArray[remainder];
   }
   output[DIGITS] = '\0';
}

int main(void)
{
  char arrOut[17];

  convertBaseVersionOne('A',arrOut );

  printf("output: %s\n", arrOut);

  return 0;
}

