#include <srxcryptoapi.h>

#if HAVE_LTDL_H
#include <ltdl.h>
int ltdl;
lt_dlhandle module;
/* libltdl is now patched, following preload symbols lines are no longer needed */

#ifdef USE_PRELOAD_SYMBOL
#ifndef lt__PROGRAM__LTX_preloaded_symbols
#define lt_preloaded_symbols    lt_libltdl_LTX_preloaded_symbols
extern LT_DLSYM_CONST lt_dlsymlist lt_libltdl_LTX_preloaded_symbols[];
#endif
#endif
#endif

void main()
{
  BGPSecKey *outKeyInfo=0;
  sca_loadKey(outKeyInfo, 0 /* pubkey */);
}
