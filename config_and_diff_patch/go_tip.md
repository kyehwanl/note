
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


4. install go1.10.2
    - Download go1.10.2.linux-amd64.tar.gz and untar
    - copy go/ into /usr/local/

    - environment variable setting for GOPATH and executable go/bin dir
        export GOPATH=/opt/project/gobgp_test/gowork
        export PATH=$PATH:$GOPATH/bin:/usr/local/go/bin
    - confirm with 'go verion'


  4.1 install go1.13
    (https://linuxize.com/post/how-to-install-go-on-centos-7/)
    - wget https://dl.google.com/go/go1.13.linux-amd64.tar.gz
    - sudo tar -C /usr/local -xzf go1.13.linux-amd64.tar.gz
    - export PATH=$PATH:/usr/local/go/bin --> store into .bash_profile



--------------------------
5. Install Latest version
	* updated on 2023.02.01
 	* updated, 2024.04.15
--------------------------
(https://go.dev/doc/install)

 * updated, 2024.04.15
 	- lastest release is v.1.22.2,  https://go.dev/dl/go1.22.2.linux-amd64.tar.gz


0. download 
		wget https://go.dev/dl/go1.19.5.linux-amd64.tar.gz


1. Remove any previous Go installation by deleting the /usr/local/go folder (if it exists), 
	then extract the archive you just downloaded into /usr/local, creating a fresh Go tree in /usr/local/go:

		$ rm -rf /usr/local/go && tar -C /usr/local -xzf go1.19.5.linux-amd64.tar.gz

	(You may need to run the command as root or through sudo).
	Do not untar the archive into an existing /usr/local/go tree. This is known to produce broken Go installations.


2. Add /usr/local/go/bin to the PATH environment variable.
	You can do this by adding the following line to your $HOME/.profile or /etc/profile (for a system-wide installation):

		export PATH=$PATH:/usr/local/go/bin

	Note: Changes made to a profile file may not apply until the next time you log into your computer. 
	To apply the changes immediately, just run the shell commands directly or execute them from the profile 
	using a command such as source $HOME/.profile.


3. Verify that you have installed Go by opening a command prompt and typing the following command:
		$ go version






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







5. go get
    go get <package>@version

    (example) $ go get rsc.io/sampler@v1.3.1

    Note the explicit @v1.3.1 in the go get argument. 
    In general each argument passed to go get can take an explicit version; the default is @latest, 
    which resolves to the latest version as defined earlier























