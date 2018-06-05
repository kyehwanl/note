#!/bin/sh
# usage : monitor_telnet.sh localhost "sh ip bgp"| telnet
echo open $1 2605
sleep 1
echo z
sleep 1
echo en
sleep 1
while :; do
    echo $2
    sleep 30
done
echo exit
