#!/bin/sh

from=$1
to=$2
name=`hostname | sed -e 's/\..*//'`
echo "quit bgpd at $name..."
#ssh $name "sudo kill -9 `pidof bgpd`"

for i in $(seq $from $to); do 
    echo node$i, quit bgpd...
    #ssh node$i "sudo kill -9 `pidof bgpd`"
    bgpd_pid=$(ssh -t node$i "pidof bgpd")
    result=$?
    if [ $result = "0" ]; then
	echo "done"
    else
	echo "once more..."
	bgpd_pid=$(ssh -t node$i "pidof lt-bgpd")
	result=$?
      	if [ $result = "0" ]; then
	    echo "done"
	    echo pid: $bgpd_pid
	fi
    fi

    ssh -t node$i "echo $bgpd_pid > temp_pid"
    ssh -t node$i "cat temp_pid"
    ssh -t node$i "kill -9 `cat ~/temp_pid`"
    ssh -t node$i "rm -rf temp_pid"
#    ssh -t node$i << !AutoStuff!
#!AutoStuff!
done
