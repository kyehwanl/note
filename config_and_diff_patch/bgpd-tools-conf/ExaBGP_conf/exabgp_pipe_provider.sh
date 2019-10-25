#!/bin/sh
 
#FIFO="/var/run/exabgp.cmd"
FIFO="/tmp/exabgp.cmd"
#trap "rm -f $FIFO" SIGINT
 
rm -f $FIFO
mkfifo $FIFO
tail -f $FIFO
