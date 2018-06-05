#!/bin/sh

#example usage: ./bunsan4.sh "vmstat 1 5" < servers.txt > output.txt


while read server; do
    ssh -n "$server" "$@" 2>&1 | sed "s/^/$server: /" &
done
wait
