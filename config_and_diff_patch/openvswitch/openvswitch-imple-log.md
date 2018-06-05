Basic configuration and compile
================================

kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ ./configure --with-linux=/lib/modules/`uname -r`/build
kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ make 
kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ sudo make install
kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ sudo make modules_install

Module loading 
===============
kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ sudo modprobe openvswitch

kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ lsmod | grep openvswitch
openvswitch           233449  0
nf_defrag_ipv6         34768  1 openvswitch
gre                    13796  1 openvswitch
ip_tunnel              23768  1 openvswitch
libcrc32c              12644  1 openvswitch
nf_defrag_ipv4         12758  2 openvswitch,nf_conntrack_ipv4
nf_conntrack           96976  7 ipt_MASQUERADE,openvswitch,nf_nat,nf_nat_ipv4,xt_conntrack,iptable_nat,nf_conntrack_ipv4

kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ dmesg | tail
[3015097.041636] nfs: server ops2.emulab2.antd.nist.gov OK
[5974211.990826] cgroup: systemd-logind (796) created nested cgroup for controller "memory" which has incomplete hierarchy support. Nested cgroups may change behavior in the future.
[5974211.990832] cgroup: "memory" requires setting use_hierarchy to 1 on the root.
[6303351.352223] gre: GRE over IPv4 demultiplexor driver
[6303351.366309] openvswitch: Open vSwitch switching datapath 2.5.0
[6303351.366419] openvswitch: LISP tunneling driver
[6303351.366421] openvswitch: GRE over IPv4 tunneling driver
[6303351.366422] openvswitch: Geneve tunneling driver
[6303351.366672] openvswitch: VxLAN tunneling driver
[6303351.366674] openvswitch: STT tunneling driver


Startup
=======

1. Initialize the configuration database using ovsdb-tool, e.g.:

      `% mkdir -p /usr/local/etc/openvswitch`  
      `% ovsdb-tool create /usr/local/etc/openvswitch/conf.db vswitchd/vswitch.ovsschema`

2. kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ sudo ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --private-key=db:Open_vSwitch,SSL,private_key --certificate=db:Open_vSwitch,SSL,certificate  --bootstrap-ca-cert=db:Open_vSwitch,SSL,ca_cert --pidfile --detach


3. Then initialize the database using ovs-vsctl.  This is only
necessary the first time after you create the database with
ovsdb-tool (but running it at any time is harmless):

      % ovs-vsctl --no-wait init


kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ ll /usr/local/var/run/openvswitch/
total 4.0K
-rw-r--r-- 1 root root 5 Aug  8 15:51 ovsdb-server.pid
srwxr-x--- 1 root root 0 Aug  8 15:51 db.sock=
srwxr-x--- 1 root root 0 Aug  8 15:51 ovsdb-server.7555.ctl=

4. main daemon start
kyehwanl@n4:~/SDN_test/openvswitch-2.5.0$ sudo ovs-vswitchd --pidfile --detach
2016-08-08T19:52:57Z|00001|ovs_numa|INFO|Discovered 8 CPU cores on NUMA node 0
2016-08-08T19:52:57Z|00002|ovs_numa|INFO|Discovered 8 CPU cores on NUMA node 1
2016-08-08T19:52:57Z|00003|ovs_numa|INFO|Discovered 2 NUMA nodes and 16 CPU cores
2016-08-08T19:52:57Z|00004|reconnect|INFO|unix:/usr/local/var/run/openvswitch/db.sock: connecting...
2016-08-08T19:52:57Z|00005|reconnect|INFO|unix:/usr/local/var/run/openvswitch/db.sock: connected
2016-08-08T19:52:57Z|00006|ofproto_dpif|INFO|system@ovs-system: Datapath supports recirculation
2016-08-08T19:52:57Z|00007|ofproto_dpif|INFO|system@ovs-system: MPLS label stack length probed as 1
2016-08-08T19:52:57Z|00008|ofproto_dpif|INFO|system@ovs-system: Datapath supports unique flow ids
2016-08-08T19:52:57Z|00009|ofproto_dpif|INFO|system@ovs-system: Datapath supports ct_state
2016-08-08T19:52:57Z|00010|ofproto_dpif|INFO|system@ovs-system: Datapath supports ct_zone
2016-08-08T19:52:57Z|00011|ofproto_dpif|INFO|system@ovs-system: Datapath supports ct_mark
2016-08-08T19:52:57Z|00012|ofproto_dpif|INFO|system@ovs-system: Datapath supports ct_label
2016-08-08T19:52:57Z|00001|ofproto_dpif_upcall(handler2)|INFO|received packet on unassociated datapath port 0
2016-08-08T19:52:57Z|00013|bridge|INFO|bridge br0: added interface br0 on port 65534
2016-08-08T19:52:57Z|00014|bridge|INFO|bridge br0: using datapath ID 0000f20d3a60ea40
2016-08-08T19:52:57Z|00015|connmgr|INFO|br0: added service controller "punix:/usr/local/var/run/openvswitch/br0.mgmt"



OtherScript log
================

1. Installation

kyehwanl@n2:~/SDN_test/openvswitch-2.5.0$ sudo make install
make  install-recursive
make[1]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0'
Making install in datapath
make[2]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath'
Making install in linux
make[3]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath/linux'
make[3]: Nothing to be done for `install'.
make[3]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath/linux'
make[3]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath'
make[4]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath'
make[4]: Nothing to be done for `install-exec-am'.
make[4]: Nothing to be done for `install-data-am'.
make[4]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath'
make[3]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath'
make[2]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0/datapath'
make[2]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0'
make[3]: Entering directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0'
 /bin/mkdir -p '/usr/local/lib'
 /bin/bash ./libtool   --mode=install /usr/bin/install -c   lib/libopenvswitch.la lib/libsflow.la ofproto/libofproto.la ovsdb/libovsdb.la vtep/libvtep.la ovn/lib/libovn.la '/usr/local/lib'
libtool: install: /usr/bin/install -c lib/.libs/libopenvswitch.lai /usr/local/lib/libopenvswitch.la
libtool: install: /usr/bin/install -c lib/.libs/libsflow.lai /usr/local/lib/libsflow.la
libtool: install: /usr/bin/install -c ofproto/.libs/libofproto.lai /usr/local/lib/libofproto.la
libtool: install: /usr/bin/install -c ovsdb/.libs/libovsdb.lai /usr/local/lib/libovsdb.la
libtool: install: /usr/bin/install -c vtep/.libs/libvtep.lai /usr/local/lib/libvtep.la
libtool: install: /usr/bin/install -c ovn/lib/.libs/libovn.lai /usr/local/lib/libovn.la
libtool: install: /usr/bin/install -c lib/.libs/libopenvswitch.a /usr/local/lib/libopenvswitch.a
libtool: install: chmod 644 /usr/local/lib/libopenvswitch.a
libtool: install: ranlib /usr/local/lib/libopenvswitch.a
libtool: install: /usr/bin/install -c lib/.libs/libsflow.a /usr/local/lib/libsflow.a
libtool: install: chmod 644 /usr/local/lib/libsflow.a
libtool: install: ranlib /usr/local/lib/libsflow.a
libtool: install: /usr/bin/install -c ofproto/.libs/libofproto.a /usr/local/lib/libofproto.a
libtool: install: chmod 644 /usr/local/lib/libofproto.a
libtool: install: ranlib /usr/local/lib/libofproto.a
libtool: install: /usr/bin/install -c ovsdb/.libs/libovsdb.a /usr/local/lib/libovsdb.a
libtool: install: chmod 644 /usr/local/lib/libovsdb.a
libtool: install: ranlib /usr/local/lib/libovsdb.a
libtool: install: /usr/bin/install -c vtep/.libs/libvtep.a /usr/local/lib/libvtep.a
libtool: install: chmod 644 /usr/local/lib/libvtep.a
libtool: install: ranlib /usr/local/lib/libvtep.a
libtool: install: /usr/bin/install -c ovn/lib/.libs/libovn.a /usr/local/lib/libovn.a
libtool: install: chmod 644 /usr/local/lib/libovn.a
libtool: install: ranlib /usr/local/lib/libovn.a
libtool: finish: PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/sbin" ldconfig -n /usr/local/lib

----------------------------------------------------------------------
Libraries have been installed in:
   /usr/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the `-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the `LD_RUN_PATH' environment variable
     during linking
   - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to `/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.

----------------------------------------------------------------------

 /bin/mkdir -p '/usr/local/bin'
  /bin/bash ./libtool   --mode=install /usr/bin/install -c utilities/ovs-appctl utilities/ovs-testcontroller utilities/ovs-dpctl utilities/ovs-ofctl utilities/ovs-vsctl utilities/ovs-benchmark ovsdb/ovsdb-tool ovsdb/ovsdb-client vtep/vtep-ctl ovn/controller/ovn-controller ovn/controller-vtep/ovn-controller-vtep ovn/northd/ovn-northd ovn/utilities/ovn-nbctl ovn/utilities/ovn-sbctl '/usr/local/bin'
libtool: install: /usr/bin/install -c utilities/ovs-appctl /usr/local/bin/ovs-appctl
libtool: install: /usr/bin/install -c utilities/ovs-testcontroller /usr/local/bin/ovs-testcontroller
libtool: install: /usr/bin/install -c utilities/ovs-dpctl /usr/local/bin/ovs-dpctl
libtool: install: /usr/bin/install -c utilities/ovs-ofctl /usr/local/bin/ovs-ofctl
libtool: install: /usr/bin/install -c utilities/ovs-vsctl /usr/local/bin/ovs-vsctl
libtool: install: /usr/bin/install -c utilities/ovs-benchmark /usr/local/bin/ovs-benchmark
libtool: install: /usr/bin/install -c ovsdb/ovsdb-tool /usr/local/bin/ovsdb-tool
libtool: install: /usr/bin/install -c ovsdb/ovsdb-client /usr/local/bin/ovsdb-client
libtool: install: /usr/bin/install -c vtep/vtep-ctl /usr/local/bin/vtep-ctl
libtool: install: /usr/bin/install -c ovn/controller/ovn-controller /usr/local/bin/ovn-controller
libtool: install: /usr/bin/install -c ovn/controller-vtep/ovn-controller-vtep /usr/local/bin/ovn-controller-vtep
libtool: install: /usr/bin/install -c ovn/northd/ovn-northd /usr/local/bin/ovn-northd
libtool: install: /usr/bin/install -c ovn/utilities/ovn-nbctl /usr/local/bin/ovn-nbctl
libtool: install: /usr/bin/install -c ovn/utilities/ovn-sbctl /usr/local/bin/ovn-sbctl
 /bin/mkdir -p '/usr/local/bin'
 /usr/bin/install -c utilities/ovs-docker utilities/ovs-pki utilities/ovs-dpctl-top utilities/ovs-l3ping utilities/ovs-parse-backtrace utilities/ovs-pcap utilities/ovs-tcpundump utilities/ovs-test utilities/ovs-vlan-test ovn/utilities/ovn-docker-overlay-driver ovn/utilities/ovn-docker-underlay-driver '/usr/local/bin'
 /bin/mkdir -p '/usr/local/sbin'
  /bin/bash ./libtool   --mode=install /usr/bin/install -c utilities/ovs-vlan-bug-workaround vswitchd/ovs-vswitchd ovsdb/ovsdb-server '/usr/local/sbin'
libtool: install: /usr/bin/install -c utilities/ovs-vlan-bug-workaround /usr/local/sbin/ovs-vlan-bug-workaround
libtool: install: /usr/bin/install -c vswitchd/ovs-vswitchd /usr/local/sbin/ovs-vswitchd
libtool: install: /usr/bin/install -c ovsdb/ovsdb-server /usr/local/sbin/ovsdb-server
 /bin/mkdir -p '/usr/local/sbin'
 /usr/bin/install -c utilities/bugtool/ovs-bugtool '/usr/local/sbin'
 /bin/mkdir -p '/usr/local/etc/bash_completion.d'
 /usr/bin/install -c utilities/ovs-appctl-bashcomp.bash utilities/ovs-vsctl-bashcomp.bash '/usr/local/etc/bash_completion.d'
/bin/mkdir -p /usr/local/var/run/openvswitch
/bin/mkdir -p /usr/local/var/lib/openvswitch/pki
/bin/mkdir -p /usr/local/var/log/openvswitch
/bin/mkdir -p /usr/local/etc/openvswitch
for plugin in utilities/bugtool/plugins/kernel-info/openvswitch.xml utilities/bugtool/plugins/network-status/openvswitch.xml utilities/bugtool/plugins/system-configuration.xml utilities/bugtool/plugins/system-logs/openvswitch.xml utilities/bugtool/plugins/system-configuration/openvswitch.xml; do \
          stem=`echo "$plugin" | sed 's,utilities/bugtool/plugins/,,'`; \
          dir=`expr "$stem" : '\(.*\)/[^/]*$'`; \
          /bin/mkdir -p "/usr/local/share/openvswitch/bugtool-plugins/$dir"; \
          /usr/bin/install -c -m 644 "./$plugin" "/usr/local/share/openvswitch/bugtool-plugins/$stem"; \
        done
/bin/mkdir -p python/ovs
sed \
                -e '/^##/d' \
                -e 's,[@]pkgdatadir[@],/usr/local/share/openvswitch,g' \
                -e 's,[@]RUNDIR[@],/usr/local/var/run/openvswitch,g' \
                -e 's,[@]LOGDIR[@],/usr/local/var/log/openvswitch,g' \
                -e 's,[@]bindir[@],/usr/local/bin,g' \
                -e 's,[@]sysconfdir[@],/usr/local/etc,g' \
                -e 's,[@]DBDIR[@],/usr/local/etc/openvswitch,g' \
                < ./python/ovs/dirs.py.template \
                > python/ovs/dirs.py.tmp
/bin/mkdir -p /usr/local/share/openvswitch/python/ovs
/usr/bin/install -c -m 644 python/ovs/dirs.py.tmp /usr/local/share/openvswitch/python/ovs/dirs.py
rm python/ovs/dirs.py.tmp
 /bin/mkdir -p '/usr/local/share/man/man1'
 /usr/bin/install -c -m 644 utilities/ovs-benchmark.1 utilities/ovs-pcap.1 utilities/ovs-tcpundump.1 ovsdb/ovsdb-tool.1 ovsdb/ovsdb-client.1 ovsdb/ovsdb-server.1 '/usr/local/share/man/man1'
 /bin/mkdir -p '/usr/local/share/man/man5'
 /usr/bin/install -c -m 644 vswitchd/ovs-vswitchd.conf.db.5 vtep/vtep.5 ovn/ovn-sb.5 ovn/ovn-nb.5 '/usr/local/share/man/man5'
 /bin/mkdir -p '/usr/local/share/man/man7'
 /usr/bin/install -c -m 644 ovn/ovn-architecture.7 '/usr/local/share/man/man7'
 /bin/mkdir -p '/usr/local/share/man/man8'
 /usr/bin/install -c -m 644 utilities/ovs-appctl.8 utilities/ovs-ctl.8 utilities/ovs-testcontroller.8 utilities/ovs-dpctl.8 utilities/ovs-dpctl-top.8 utilities/ovs-l3ping.8 utilities/ovs-ofctl.8 utilities/ovs-parse-backtrace.8 utilities/ovs-pki.8 utilities/ovs-vlan-bug-workaround.8 utilities/ovs-test.8 utilities/ovs-vlan-test.8 utilities/ovs-vsctl.8 utilities/bugtool/ovs-bugtool.8 vswitchd/ovs-vswitchd.8 vtep/vtep-ctl.8 ovn/controller/ovn-controller.8 ovn/controller-vtep/ovn-controller-vtep.8 ovn/northd/ovn-northd.8 ovn/utilities/ovn-ctl.8 ovn/utilities/ovn-nbctl.8 ovn/utilities/ovn-sbctl.8 '/usr/local/share/man/man8'
 /bin/mkdir -p '/usr/local/share/openvswitch'
 /bin/mkdir -p '/usr/local/share/openvswitch/python/ovs/db'
 /usr/bin/install -c -m 644  python/ovs/db/__init__.py python/ovs/db/data.py python/ovs/db/error.py python/ovs/db/idl.py python/ovs/db/parser.py python/ovs/db/schema.py python/ovs/db/types.py '/usr/local/share/openvswitch/python/ovs/db'
 /bin/mkdir -p '/usr/local/share/openvswitch/python/ovs/unixctl'
 /usr/bin/install -c -m 644  python/ovs/unixctl/__init__.py python/ovs/unixctl/client.py python/ovs/unixctl/server.py '/usr/local/share/openvswitch/python/ovs/unixctl'
 /bin/mkdir -p '/usr/local/share/openvswitch/python/ovstest'
 /usr/bin/install -c -m 644  python/ovstest/__init__.py python/ovstest/args.py python/ovstest/rpcserver.py python/ovstest/tcp.py python/ovstest/tests.py python/ovstest/udp.py python/ovstest/util.py python/ovstest/vswitch.py '/usr/local/share/openvswitch/python/ovstest'
 /bin/mkdir -p '/usr/local/share/openvswitch/python/ovs'
 /usr/bin/install -c -m 644  python/ovs/__init__.py python/ovs/daemon.py python/ovs/fatal_signal.py python/ovs/json.py python/ovs/jsonrpc.py python/ovs/ovsuuid.py python/ovs/poller.py python/ovs/process.py python/ovs/reconnect.py python/ovs/socket_util.py python/ovs/stream.py python/ovs/timeval.py python/ovs/util.py python/ovs/version.py python/ovs/vlog.py '/usr/local/share/openvswitch/python/ovs'
 /bin/mkdir -p '/usr/local/include/openflow'
 /usr/bin/install -c -m 644 include/openflow/netronome-ext.h include/openflow/nicira-ext.h include/openflow/openflow-1.0.h include/openflow/openflow-1.1.h include/openflow/openflow-1.2.h include/openflow/openflow-1.3.h include/openflow/openflow-1.4.h include/openflow/openflow-1.5.h include/openflow/openflow-common.h include/openflow/openflow.h '/usr/local/include/openflow'
 /bin/mkdir -p '/usr/local/include/openvswitch'
 /usr/bin/install -c -m 644 include/openvswitch/compiler.h include/openvswitch/list.h include/openvswitch/thread.h include/openvswitch/token-bucket.h include/openvswitch/types.h include/openvswitch/util.h include/openvswitch/version.h include/openvswitch/vconn.h include/openvswitch/vlog.h '/usr/local/include/openvswitch'
 /bin/mkdir -p '/usr/local/lib/pkgconfig'
 /usr/bin/install -c -m 644 ./lib/libopenvswitch.pc ./lib/libsflow.pc ./ofproto/libofproto.pc ./ovsdb/libovsdb.pc '/usr/local/lib/pkgconfig'
 /bin/mkdir -p '/usr/local/share/openvswitch'
 /usr/bin/install -c -m 644 vswitchd/vswitch.ovsschema vtep/vtep.ovsschema ovn/ovn-sb.ovsschema ovn/ovn-nb.ovsschema '/usr/local/share/openvswitch'
 /bin/mkdir -p '/usr/local/share/openvswitch/scripts'
 /usr/bin/install -c -m 644 utilities/ovs-lib '/usr/local/share/openvswitch/scripts'
 /bin/mkdir -p '/usr/local/share/openvswitch/scripts'
 /usr/bin/install -c utilities/ovs-check-dead-ifs utilities/ovs-ctl utilities/ovs-save utilities/bugtool/ovs-bugtool-bfd-show utilities/bugtool/ovs-bugtool-cfm-show utilities/bugtool/ovs-bugtool-coverage-show utilities/bugtool/ovs-bugtool-fdb-show utilities/bugtool/ovs-bugtool-lacp-show utilities/bugtool/ovs-bugtool-list-dbs utilities/bugtool/ovs-bugtool-memory-show utilities/bugtool/ovs-bugtool-tc-class-show utilities/bugtool/ovs-bugtool-vsctl-show utilities/bugtool/ovs-bugtool-ovsdb-dump utilities/bugtool/ovs-bugtool-daemons-ver utilities/bugtool/ovs-bugtool-ovs-ofctl-show utilities/bugtool/ovs-bugtool-ovs-ofctl-dump-flows utilities/bugtool/ovs-bugtool-ovs-appctl-dpif utilities/bugtool/ovs-bugtool-bond-show utilities/bugtool/ovs-bugtool-conntrack-dump vtep/ovs-vtep ovn/utilities/ovn-ctl '/usr/local/share/openvswitch/scripts'
make[3]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0'
make[2]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0'
make[1]: Leaving directory `/users/kyehwanl/SDN_test/openvswitch-2.5.0'
