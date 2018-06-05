#!/bin/sh

#example usage: ./bunsan3.sh free < servers.txt


while read server; do
    ssh -n "$server" "$@" 2>&1 | sed "s/^/$server: /"
done
