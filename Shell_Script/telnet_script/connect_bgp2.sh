#!/bin/sh
# usage: connect_bgp2.sh |  telnet
echo open node4 2605
sleep 1
echo zebra
sleep 1
echo en
sleep 1
echo conf t
sleep 1
echo router bgp 60005
sleep 1
echo srx connect bgp2.antd.nist.gov 17900
sleep 1
echo exit
