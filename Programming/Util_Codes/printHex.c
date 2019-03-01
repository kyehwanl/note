
__attribute__((always_inline)) inline void printHex(int len, unsigned char* buff)
{
  int i;
  for(i=0; i < len; i++ )
  {
      if(i%16 ==0) printf("\n");
      printf("%02x ", buff[i]);
  }
  printf("\n");
}
