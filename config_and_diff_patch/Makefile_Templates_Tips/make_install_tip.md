

Comparison 'make install' with 'prefix' and 'DESTDIR'
====================================================


https://docs.google.com/document/d/1cYPeaB3sIQDs1w5iHRcWM7lvher9-2bapxj0e38GUmQ/edit#


< short summary >

in case, ./configure --prefix=/dir1/dir2

1. make install prefix=/dir3

    /dir3/bin 
    /dir3/include
    /dir3/lib


2. make install DESTDIR=/dir4/dir5/

    /dir4/dir5/dir1/dir2/bin 
    /dir4/dir5/dir1/dir2/include
    /dir4/dir5/dir1/dir2//lib

