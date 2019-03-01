AC_PREREQ([2.63])
AC_INIT([SRx], [0.3.0.1], [bgpsrx-dev@nist.gov])
AC_PROG_MAKE_SET
AC_CONFIG_MACRO_DIR([m4])

AC_MSG_CHECKING([if iconv uses const pointers])
AC_COMPILE_IFELSE(  [AC_LANG_PROGRAM([[#include <iconv.h>]],
                                     [[
                                       const char *fromPtr=0;
                                       size_t     fromLen=0;
                                       char       *toPtr=0;
                                       size_t     toLen=0;
                                       iconv_t    cv=0;
                                       iconv(cv, &fromPtr, &fromLen, &toPtr, &toLen);
                                      ]])],
                    [
                      AC_MSG_RESULT([yes])
                      AC_DEFINE_UNQUOTED([ICONV_USES_CONST_POINTER], 1, [Define to 1 if you have to use const char* with iconv, to 0 if you must use char*.])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([ICONV_USES_CONST_POINTER], 0, [Define to 1 if you have to use const char* with iconv, to 0 if you must use char*.])
                    ]
                 )
AC_MSG_CHECKING([for wcsrtombs])
AC_RUN_IFELSE(  [AC_LANG_PROGRAM([[#include <wchar.h>
#include <string.h>]],
                                 [[
mbstate_t st;
memset(&st, 0, sizeof(st));
char buffer[32];
const wchar_t* src=L"help";
wcsrtombs(buffer, &src, 32, &st);
if(src==0)
    return 0;
else
    return 1;]])],
                    [
                      AC_MSG_RESULT([yes --])
                      AC_DEFINE_UNQUOTED(
                                         [HAVE_WCSRTOMBS], 1, 
                                         [Define to 1 if you have the "wcsrtombs" function])
                    ],
                    [
                      AC_MSG_RESULT([no])
                      AC_DEFINE_UNQUOTED([HAVE_WCSRTOMBS], 0, [Define to 1 if you have the "wcsrtombs" function.])
                    ]
                 )

AC_MSG_CHECKING([ srx cryptoapi testing])
save_libs="$LIBS"
save_ldflags="$LDFLAGS"
LIBS="$LIBS  -ldl -lSRxCryptoAPI"
LDFLAGS="$LDFLAGS -L/users/kyehwanl/proj-bgp/extras/srxcryptoapi/test_install/lib/ -Wl,-rpath -Wl,/users/kyehwanl/proj-bgp/extras/srxcryptoapi/test_install/lib"
CFLAGS="$CFLAGS -I/users/kyehwanl/proj-bgp/extras/srxcryptoapi/test_install/include/"


#AC_RUN_IFELSE( [AC_LANG_PROGRAM([[#include <srxcryptoapi.h>]],
AC_LINK_IFELSE( [AC_LANG_PROGRAM([[#include <srxcryptoapi.h>]],
                                [[
BGPSecKey *outKeyInfo=0;
sca_loadKey(outKeyInfo, 0 );
                                 ]])],
               [AC_MSG_RESULT(srxcprytoapi found and link success -- yes)],
               [AC_MSG_RESULT(srxcryptoapi found and link success -- no)
               LIBS="$save_libs" LDFLAGS="$save_ldflags"]
              )
LIBS="$save_libs"
LDFLAGS="$save_ldflags"


AC_OUTPUT
