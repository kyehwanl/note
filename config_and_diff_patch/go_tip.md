
installation
============
1. download 
 Download the zip or tarball of Go 1.4 for your platform and extract it to $HOME/go1.4 (or your nominated GOROOT_BOOTSTRAP location

   - wget https://storage.googleapis.com/golang/go1.4.linux-386.tar.gz
   - set up GOROOT_BOOTSTRAP as a env variable
        * touch ~/.go
        * input "export GOROOT_BOOTSTRAP=~/Download/go1.4"
        * input "source ~/.go" into .bashrc

   - complete .go environment
   export GOPATH=/users/kyehwanl/srx_test1/test/gowork
   export GOROOT_BOOTSTRAP=~/Download/go1.4
   export PATH=$PATH:/users/kyehwanl/Download/go1.4/bin


2. install go1.4

    - cd src/ 
    - ./all.bash

    [kyehwanl@node2 src]$ ./all.bash
    ...
    
    # Checking API compatibility.
     Skipping cmd/api checks; hg not available
     
     real    0m0.705s
     user    0m0.471s
     sys     0m0.124s
     
     ALL TESTS PASSED
     
     ---
     Installed Go for linux/386 in /users/kyehwanl/Download/go1.4
     Installed commands in /users/kyehwanl/Download/go1.4/bin
     *** You need to add /users/kyehwanl/Download/go1.4/bin to your PATH.`


3. install go1.7

  - download go1.7 and untar
  - cd src and ./all.bash


  [kyehwanl@node2 src]$ ./all.bash
  ##### Building Go bootstrap tool.
  cmd/dist

  ##### Building Go toolchain using /users/kyehwanl/Download/go1.4.
  bootstrap/internal/sys
  bootstrap/asm/internal/flags
  bootstrap/internal/bio


Error 
======
If the following error occurs, 

    runtime: may need to increase max user processes (ulimit -u)
    fatal error: newosproc

then, need to increase the number of processes to be handled same time

    ulimit -u 2048





Result
======
    ##### API check
    Go version is "go1.7", ignoring -next /users/kyehwanl/Download/go/api/next.txt

    ALL TESTS PASSED

    ---
    Installed Go for linux/386 in /users/kyehwanl/Download/go
    Installed commands in /users/kyehwanl/Download/go/bin
