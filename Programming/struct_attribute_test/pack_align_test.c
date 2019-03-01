#include <stdio.h>

void main(void)
{

  struct PACK1
  {
    char c1;
    int var1;
  } __attribute__((packed));


  struct PACK2
  {
    char c1;
    int var1;
  };

  struct ALIGN1
  {
    char c1;
    int var1;
  } __attribute__((aligned(4)));

  struct ALIGN2
  {
    char c1;
    int var1;
  };

  struct ALIGN_16
  {
    char c1  __attribute__((aligned(16)));
    int var1 __attribute__((aligned(16)));
  };

  struct PACK1 pack1;
  struct PACK2 pack2;
  struct ALIGN1 align1;
  struct ALIGN2 align2;

  memset(&pack1, 0x0, sizeof(struct PACK1));
  memset(&pack2, 0x0, sizeof(struct PACK2));
  memset(&align1, 0x0, sizeof(struct ALIGN1));
  memset(&align2, 0x0, sizeof(struct ALIGN2));

  pack1.c1 = 0xAA;
  pack1.var1 = 0xABCD1234;

  align1.c1 = 0xBB;
  align1.var1 = 0x56781234;


  printf("size of PACK1: %d | ALIGN1: %d\n", sizeof(struct PACK1), sizeof(struct ALIGN1));
  printf("size of pack1 : %d\n", sizeof(pack1));
  printf("size of align1 : %d\n", sizeof(align1));


  // TODO: try to gdb and look into memory allocation
  memcpy(&pack2, &pack1, sizeof(pack1));
  memcpy(&align2, &align1, sizeof(align1));
  //pack2 = pack1; --> error: incompatible types when assigning to type ‘struct PACK2’ from type ‘struct PACK1


  /* structural copy */
  struct PACK1 pack3;
  struct ALIGN1 align3;

  // TODO: try to gdb and look into memory allocation
  pack3 = pack1;
  align3 = align1;


  // TODO: try to gdb and look into memory allocation
  struct ALIGN_16 align16;
  memcpy(&align16, &align1, sizeof(align1));



}
