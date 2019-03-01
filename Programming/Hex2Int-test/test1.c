#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

char bgpsec_ski[40] = "C30433FA1975FF193181458FB902B501EA9789DC";
void main(void)
{
  u_int8_t bski[20];
  u_int8_t ski[20];
  int i;
  char temp[2];
  u_int8_t num;

  sscanf(bgpsec_ski,\
        "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x", \
        bski,(bski+1),(bski+2),(bski+3),(bski+4),\
        (bski+5),(bski+6),(bski+7),(bski+8),(bski+9),\
        (bski+10),(bski+11),(bski+12),(bski+13),(bski+14),\
        (bski+15),(bski+16),(bski+17),(bski+18),(bski+19));

  char *endptr=NULL;
  printf("endptr initial address:%p  ski:%p temp:%p\n", endptr, ski, temp);
  for (i=0; i<20; i++)
  {
    memcpy(temp, &bgpsec_ski[i*2], 2);
    num = ski[i] =(u_int8_t) strtoul(temp, &endptr, 16);
    printf("%02x ", num);
  }


  for(i=0; i<20; i++)
    printf("%02x ", ski[i]);
  printf("\n");

  for(i=0; i<20; i++)
    printf("%02x ", bski[i]);
  printf("\n");

}
