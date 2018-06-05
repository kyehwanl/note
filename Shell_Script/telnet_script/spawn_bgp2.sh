#!/bin/sh
echo open bgp2.antd.nist.gov 2605
sleep 1
echo zebra
sleep 1
echo en
sleep 1
echo conf t
sleep 1
echo router bgp 34
sleep 1
echo srx connect localhost 17900
sleep 1
echo exit
