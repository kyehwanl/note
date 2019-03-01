AC_PREREQ([2.63])
AC_INIT([SRx], [0.3.0.1], [bgpsrx-dev@nist.gov])
AC_PROG_MAKE_SET
AC_CONFIG_MACRO_DIR([m4])


AM_PROG_AR
#CFLAGS="$CFLAGS -I/tmp/nist-srx-bundle-22-0.4.1.3/srxcryptoapi-0.1.1.1/test_install/include"
#LDFLAGS="$LDFLAGS -L/tmp/nist-srx-bundle-22-0.4.1.3/srxcryptoapi-0.1.1.1/test_install/lib/"
AC_MSG_CHECKING([Linux distribution checking])
cat /proc/version | grep -i ubuntu >/dev/null 2>&1
if [[ $? -eq 0 ]]; then
  OS_DIST=ubuntu
else
  OS_DIST=other
fi
AC_MSG_RESULT([$OS_DIST])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])


dnl
dnl If this compilation test pass through, that means ltdl.h will be included in config.h
dnl
AC_MSG_CHECKING([compiling test for ltdl])
header_ltdl=no
AC_COMPILE_IFELSE(
                  [AC_LANG_PROGRAM( [[
                   #include<ltdl.h>
                   ]],
                   [[ ]])],
                  [header_ltdl=yes],[]
                 )
AC_MSG_RESULT(ltdl header: $header_ltdl)


save_libs="$LIBS"
LIBS="$LIBS  -lltdl"
need_preload_symbol=no
if test "x$OS_DIST" = xubuntu; then
  AC_MSG_CHECKING([libtool dynamic loader link availability])
  AC_LINK_IFELSE( [AC_LANG_PROGRAM([[
                   #include<ltdl.h>
                   ]],
                   [[
                    int ltdl;
                    LTDL_SET_PRELOADED_SYMBOLS();
                    ltdl = lt_dlinit();
                    ]])],
                  [AC_MSG_RESULT(availability: yes)],
                  [AC_MSG_RESULT(availability: no)
                  need_preload_symbol=yes]
                )
fi
LIBS="$save_libs"

dnl
dnl In case of success, USE_PRELOAD_SYMBOL enables the patch code to be able to link
dnl
available_preload_symbol=no
AC_MSG_CHECKING([ltdl patch capability])
if test "x$need_preload_symbol" = xyes; then
  if test "x$header_ltdl" = xyes; then
    save_libs="$LIBS"
    LIBS="$LIBS -lltdl"
    AC_LINK_IFELSE(
                   [AC_LANG_PROGRAM( [[
                    #include<ltdl.h>
                    ]],
                    [[
                     #ifndef lt__PROGRAM__LTX_preloaded_symbols
                     #define lt_preloaded_symbols    lt_libltdl_LTX_preloaded_symbols
                     extern lt_dlsymlist lt_libltdl_LTX_preloaded_symbols[];
                     #endif
                     int ltdl;
                     LTDL_SET_PRELOADED_SYMBOLS();
                     ltdl = lt_dlinit();
                    ]])
                   ],
                   [AC_MSG_RESULT(capability: yes)
                   LIBS="$save_libs"
                   available_preload_symbol=yes
                   AC_DEFINE([USE_PRELOAD_SYMBOL], [], [enable ltdl preload symbol])
                   AC_CHECK_HEADERS([ltdl.h])
                   AC_SEARCH_LIBS([lt_dlopen], [ltdl])
                   ],
                   [LIBS="$save_libs"
                    AC_MSG_WARN([ warning -- need to use dl library instead of ltdl ])
                   ]
                  )
  fi
fi
AC_MSG_CHECKING([preload symbol status])
AC_MSG_RESULT([need: $need_preload_symbol  avaliable: $available_preload_symbol])


AC_OUTPUT
