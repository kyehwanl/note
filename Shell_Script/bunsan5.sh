#!/bin/sh

#usage: ./bunsan5.sh servers.txt free

file=$1
shift

while read server; do 
    ssh -n "$server" "$@" 2>&1 | sed "s/^/$server: /" &
done < "$file"
wait

