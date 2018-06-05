#!/bin/sh

var=$1
: ${var:=nothing}
if [ "$var" = "nothing" ]; then
    echo " usage: srx-connect.sh <IP_address or hostname> | telnet"
    echo 
    exit
fi

srx_server_node=10.0.30.101

case "$1" in 
  node1 ) 
    echo open 10.0.30.75 2605
    router_id=60012
    ;;
  node2 ) 
    echo open 10.0.30.82 2605
    router_id=60013
    ;;
  node3 ) 
    echo open 10.0.30.84 2605
    router_id=60014
    ;;
  node4 ) 
    echo open 10.0.30.70 2605
    router_id=60015
    ;;
  node5 ) 
    echo open 10.0.30.72 2605
    router_id=60016
    ;;
  node6 ) 
    echo open 10.0.30.67 2605
    router_id=60017
    ;;
  * ) 
    echo open $1 2605
    ;;
esac

sleep 1
echo zebra
sleep 1
echo en
sleep 1
echo conf t
sleep 1
echo router bgp $router_id
sleep 1
echo srx connect $srx_server_node 17900
sleep 1
echo exit
