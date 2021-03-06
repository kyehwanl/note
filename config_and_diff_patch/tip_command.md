(1) cscope & ctags
====================

find ./ -name '*.[ch]' | ctags -R -L -
find ./ -name '*.[ch]' > cscope.files (or when you exclude something, use 'grep -v "something"')
 e.g., --> find ./ -name '*.[ch]' | grep -v 'test_install'
cscope -b

 <CAUTION>
 If vim cannot find headers or links, try to use physical path($PWD) instead of relative path(./) in 'find' command.


(2) diff & patch
==================
find ./ -name '*.[ch]' -exec diff -wibuN {} ../0198-09-KH/{} \; > patch_09
find ./ -type f \( -name '*.[ch]' -o -name 'Makefile.am' \) -exec file {} \; | less

#practical example :  
    If there are two old-verion/  new-version/ directories,
    then "cd old-version" and command following line.

    find ./ -type f \( -name '*.[ch]' -o -name 'Makefile.am' \) -exec diff -wibuN {} ../new/{} \; > patch.diff
    diff --exclude=.p4config -uNR quagga/Makefile.am quagga-mpls/Makefile.am

 <CAUTION>
 if you just want to know what differences between orig and new(not patch purpose), just do diff without -N option. 
 Because -N option will generate lot's of new file lines unless either directory doesn't have same file.

#practical examples with Regular expression in Find
find -name "*.xls" -o -name "*.csv" ==  find -regex ".*\.\(xls\|csv\)"


(3) git 
==================
1. <<find a certain file>>
 $ git diff --diff-filter=D --name-only HEAD@{1} | grep "php"

2. <<git diff or log>>
 $ git log -p -3 -- $(for i in $(git ls-files); do if [ "${i%/*}" = "AS50/srx/src/server" ]; then echo $i; fi; done )
 $ git diff 0f01  -- $(for i in $(git ls-files); do if [ "${i%/*}" = "AS50/srx/src/server" ]; then echo $i; fi; done )

 $ git log -p -3 -w -- $(for i in $(git ls-files); do if [ "${i%/*}" = "AS50/srx/src/server" ]; then if [ "${i#*.}" = "c" ] || [ "${i#*.}" = "h" ]; then echo $i; fi; fi; done)
 $ git diff -w fafd4 -- $(for i in $(git ls-files); do if [ "${i%/*}" = "AS50/srx/src/server" ]; then if [ "${i#*.}" = "c" ] || [ "${i#*.}" = "h" ]; then echo $i; fi; fi; done) 
 $ git log -p -1 -w -- $(for i in $(git ls-files); do if [ "${i%/*}" = "020-03/quagga-0.99.16-srx-0.2/bgpd" ]; then echo $i; fi; done)

 $ git diff -w -b cf5ca40 -- $(for i in $(git ls-files); do if [ "${i%/*}" = "srx-0.2.0/src/client" ] || [ "${i%/*}" = "quagga-0.99.16-srx-0.2/bgpd" ]; then if [ "${i##*.}" = "c" ] || [ "${i##*.}" = "h" ]; then echo $i; fi; fi; done)

3. << compare a file between different index >>
 $ git diff -b regiQ_mthread..regiQ_comm_dgram_nonblk -- srx-0.2.0/src/client/client_connection_handler.c

4. simple form
 $ git log -p -1 -- $(git ls-files | grep \.[ch]$)

5. any commits in your current branch that aren't in the master branch on your origin remote, and those commits will be transferred to the server
 $ git log origin/master..HEAD

6. (cf) triple dots : 
    - to find the diff from their common ancestor to test
    - specifies all the commits that are reachable by either of two references but not by both of them

7. reverse order of branches
 $ git log --oneline --decorate  $(git rev-list --branches --no-walk) 
 
8. show branches relations in tree expression
 $ git log --graph --oneline --decorate --branches
9. extract a single file from a specific revision
 $ git show $REV:$FILE > file


(4) vim -substitution
======================
#1 substitution "--- ./rip..."  with "--- ../quagga-0.99.16/rip..."
:1,$ s/---\ \.\//---\ \..\/quagga-0.99.16\//g


(5) symbolic link 
==================
-- making symbolic link
 ln -sf <TARGET> <LINK_NAME>

-- removing symbolic link
 rm <LINK_NAME>

 <CAUTION>
 When removing, prevent from adding '/' at the end of <link name>, 
 otherwise "rm: cannot remove `<link name>/': Not a directory"


(6) binutils
==================
1. objdump -Sl --no-show-raw-insn lt-bgpd --start-address=0x4268c | less
2. addr2line -f -e lt-bgpd 0xb7fa36e4 : find a function name using address


(7) vertical select
=====================
1. cut -c '9[-][number]' origianl.txt > new.txt: (ex. cut -c '9-' temp>new)
2. in Vim, ctrl+v, then select


(8) ssh tips
=================================
ssh -t -t -o StrictHostKeyChecking=no ops.emulab.antd.nist.gov
ssh-keygen & ssh-copy-id <server>
ssh -o ServerAliveInterval=100 <host>


(9) svn tips
================
1. commit only modified files 
    svn status | egrep '^M.*\.[ch]$' | sed -e 's/^. *//' > tmp.txt && svn ci --targets tmp.txt
2. SSL/TLS Error with a Subversion(SVN) Client
    svn switch --relocate https://svn.olddomain.com https://svn.newdomain.com
3. upload mydir under the server's any_dir_name directory
    svn import mydir/ https://is1.antd.nist.gov/svn/users/kyehwanl/any_dir_name


(10) tar - extract certain files
=================================
tar xvfz ts1_srx.git_03-19-2012.tar.gz --wildcards --no-anchored 'php*'


(11) gdb
==========
1. set arg <arg1> <arg2> ... <arg n>
2. set directories <source directory>
3. set step-mode on : setep command enter a certain function  even though the function was not built with -g(no debug information)
4. set stop-on-solib-event 1 : stop execution when the controlled program loads a new shared library
5. graph display 'x/48wx$esp'
6. p *pt@4 (pt: point of a struct array which has 4 array elements)

++ <TUI mode> --> refer to gdb_tips
press '-' on gdb running (tui enable|disable)
1. layout|focus <src|asm|split|regs>
2. winheight <name> <[+|-]count>
3. update: current win & execution point, refresh(Ctrl-L), info win


(12) yum rpm
=============
1. making(updating) a  yum repository : createrepo [--update] /path/to/repo 
2. download a source rpm: yumdownloader --source <package_name>
3. extract files from rpm : rpm2cpio <src.rpm> | cpio -idmv
4. yum --disablerepo="*" --enablerepo="srx" list available
5. download rpm :
    1st check to see if you have following plugin - yum install yum-plugin-downloadonly
    2nd - yum install --downloadonly [--downloaddir=<path>] <package>


(13) packet capture
====================
1. tshark: sudo tshark -i eth23 -nn -f"tcp port 179" -x -V -R"bgp.type==2 || bgp.type==1"
           sudo tshark -i veth0 -nn -f"tcp port 179" -x -V -Y"bgp.type==2 || bgp.type==1 and ip.src==10.1.1.1"
2. sudo tcpdump -i eth23 -X -nnvv port 179 and \(tcp[0x54:1] == 0x02 \)
3. capture traffic remotely over ssh: ssh node2 'sudo tshark -i eth30 -nn -f"tcp port 179" -w -' | wireshark -k -i -
    if "sudo: sorry, you must have a tty to run sudo" problem --> comment out "Default requiretty" in /etc/sudoers


(14) rsync
====================
1. backup:  rsync -avP --delete <src> <dest>  
(example) rsync -avP config_and_diff_patch/tip_command bgp2.antd.nist.gov:~/config_and_diff_patch/
2. view and diff: rsync -avni <src> <dest>
    <cf> -i(--itemize-changes): YXcstpoguax -- Y(<):sent(to remote), X(f):file, c:checksum, s:size, t:time, p:permission, o:owner...)
3. to transfer the specified files
  $  find ./ -name '*.[ch]' -print0 | rsync -avPni --files-from=- --from0 <source> <target>
  $  rsync -avPni  <source> <target>  --exclude='.git*' --exclude='test_install' --exclude="quaggasrx/[^b]*" --include="*/" --include="*.[ch]"  --exclude="*" --prune-empty-dirs
4. using ssh-tunnel ( using -u, --update :  skip files that are newer on the receiver)
  $ rsync -avPuni -e 'ssh -p 12345' kyehwanl@localhost:/users/kyehwanl/config_and_diff_patch/ ./config_and_diff_patch/

(15) grep | egrep
====================
1. recursive grep for a particular file type: 
    grep -lr --include='*.[ch]' search-pattern ./
2. exclude leading # or empty lines in file
    grep -v -E ‘^\#|^$’ /etc/squid/squid.conf


(16) ssh tunnel
====================
1. ssh -f -N -L12345:node5.ex1-test.bgp-srx.emulab.antd.nist.gov:22 bgp2.antd.nist.gov
2. ssh -p 12345 -o StrictHostKeyChecking=no kyehwanl@localhost


(17) compare two files or dirs
===============================
1. diff -qr ~peter ~george  | sort
   <example> diff -qr -x "*.o" -x "*.a" . ~/srx_test1/git.svn/quagga-gitsvn/bgpd | sort
2. vimdiff using scp
3. rsync using -ni
   (example) rsync -avPni ./*.[ch] ~/srx_test1/git.svn/quagga-gitsvn/bgpd/ | egrep '^.f\.s'


(18) redirect output from a running process
============================================
1. using gdb - A
    'ctrl-z' and 'bg' and 'touch /tmp/stdout'
    gdb -p $! ($!: background process)
    p dup2(open("/tmp/stdout", 1), 1)  [or creat("/tmp/stdout", 0600) instead of open()]
    detach and quit from gdb
    'disown' and 'logout' back in the shell [disown: avoiding SIGHUP from shell logout]
2. using gdb -B
    get running pts# with 'ps aux' and yours with 'ps -t'
    gdb -p <pid> then, 'p dup2(open("/dev/pts/<yours>", 1), 1)' ; then detach, quit
2. strace -e write=1,2 -e trace=write -p <pid>


(19) apt-get dpkg
==================
0. /etc/apt/sources.list : remote package repository
1. repository update: apt-get update    package upgrage: apt-get upgrade
2. apt-get install <package name>   apt-get [--purge] remove (purge removes all conf files, too)
3. source download: apt-get source <package name>
4. apt-cache show <package name>    apt-cache search <search string>
5. list of installed packages: dpkg -l   dpkg -L <pkg name>: list of all files of pkg
6. .deb install: dpkg -i <deb pkg>   (ignore dependencies: --force-depends)
7. GUI: gnome: synaptic, kde: muon


(20) lsof
==========
1. lsof -c <program>    lsof -i  (internet sockets)
2. lsof -p <PID>    lsof  -u (uid)
3. lsof -i[tcp|udp][@address or ip][:port]
  (example) lsof -i:6633    lsof -iTCP(|UDP)@security.kaist.ac.kr:smtp


(21) dynamic-loader(ld.so) search path
=======================================
1. gcc -Wl, --verbose  (--> linker path, not dynamic-loader)
2. ld --verbose | grep SEARCH_DIR | tr -s ' ;' \\012 or \\n  (--> linker search path)
3. ldconfig -v 2>/dev/null | grep -v ^$'\t'
4. gcc -print-search-dirs | sed '/^lib/b 1;d;:1;s,/[^/.][^/]*/\.\./,/,;t 1;s,:[^=]*=,:;,;s,;,;  ,g' | tr \; \\012
5. gcc -m64 -Xlinker --verbose  2>/dev/null | grep SEARCH | sed 's/SEARCH_DIR("=\?\([^""]\+\)"); */\1\n/g'  | grep -vE '^$'



(ETC)
=================================
A. show file types except source files in project directory
    for i in $(git ls-files); do if [ -z "$(file $i | grep ASCII)" ]; then file $i; fi done
B. killing all bgpd processes
    for i in $(ps aux | grep bgpd | grep -v grep | awk '{print $2}'); do echo $i; sudo kill -9 $i; done
C. grep useful option : -RInHw
    R: recursive, I: skip binary file, n: line number, H: file name, w: whole word
D. for i in $(ls *.xml); do newfile=$(echo $i | sed -e s/\:/\./g); mv $i $newfile; done
E. calculation using 'bc'
   > echo "ibase=16; FFFFFFFF*FF*2" | bc
   > echo "obase=16; 408" | bc --> 198
   > echo "(" $(cat /proc/vmallocinfo | grep -v "ioremap" | awk '{print $2}' | paste -s -d "+") ")/ 1024" | bc 
F. bash shell and gdb batch-mode calculation
   $((10+1))
   $gdb -q --batch -ex "p 0xaa - 52" -ex 'p 0x82'
G. connect pipeline same time (T shape)
    --> date 2>&1 | tee logfile.txt | logger -t "header"
H. String Manipulation --> refere to FILE(string_manipulation_tip)
   > curl -s -I http://bgp2.antd.nist.gov | head -n 1 | cut -f 2 -d ' '
   > ps -ef > /tmp/tmp0; cat /tmp/tmp0 | tr -s " " | cut -d " " -f 2 | grep -v PID
   > cat /etc/passwd | awk -F ":" '{print $1}'
   > yes "kill" | head -$(cat /tmp/tmp0 | wc -l) | paste -d " " - /tmp/tmp0 > /tmp/tmp1
    (in paste command, there 's '-', it means using a standard input instead of a file input)
   > sudo kill -9 $(ps aux | grep bgp[d] | awk '{print $2}' | paste -s -d ' ')
   > remove brackets([ ]) --> tr -d '[]'
   > replace '-' with '_' --> echo "some_thing" | tr '_' '-' --> some-thing
I. rpm macro
  > rpmbuild --nodeps --buildroot="${PWD}/_rpm" -ba --define="_rpmdir ${PWD}" --define="_srcrpmdir ${PWD}" --define="_sourcedir ${PWD}" --define="_specdir ${PWD}" --define="_builddir ${PWD}" srxcryptoapi-0.1.1.0.spec
J. compare files over ssh with vimdiff
   vimdiff config_and_diff_patch/tip_command scp://bgp2.antd.nist.gov//users/kyehwanl/config_and_diff_patch/tip_command
    or scp://bgp2.antd.nist.gov/~/config_and_diff_patch/tip_command
   AND (in vim command mode) :vsp scp://ts1/~/Projects/proj-bgp/quaggasrx/test_install/etc/bgpd.conf
K. NFS mount: mount -t nfs -o rsize=8192,wsize=8192,nfsvers=3 fs5-141:/users/kyehwanl /users/kyehwanl
L. iptables(bgp) iptables -I INPUT 1 -p tcp --dport 179 -j ACCEPT
M. X11 configuration problem: (1) goto root with sudo -s, (2) xauth merge /users/kyehwanl/.Xauthority
N. Convert VirtualBox image to KVM: qemu-img convert mininet-vm-x86_64.vmdk -O qcow2 Mininet-VM.qcow2
   How to run: kvm -m 2048 -net nic,macaddr=00:11:22:CC:CC:10 -net tap Mininet-VM.qcow2
O. change mac address:  ifconfig s1 hw ether 3e:db:b0:4c:74:47






