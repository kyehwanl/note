#include <srxcryptoapi.h>

void main()
{
  BGPSecKey *outKeyInfo=0;
  sca_loadKey(outKeyInfo, 0 /* pubkey */);
}
