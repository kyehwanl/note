#!/bin/sh 
 
name=`hostname | sed -e 's/\..*//'`

function run_normal()
{
  set -x
  sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -i /tmp/bgpd.pid -f ~/srx_test1/$name/bgpd.conf
  #set +x
}

function run_test()
{
  set -x
  sudo ~/srx_test1/test/qsrx/bgpd/.libs/bgpd -i /tmp/bgpd.pid -f ~/srx_test1/$name/bgpd.conf
  #set +x
}

var=$1
: ${var:=nothing}

if [ "$var" = "nothing" ]; then
  run_normal
elif [ "$var" = "test" ]; then
  run_test
fi
