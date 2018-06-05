#!/bin/sh

# usage: ./bunsan1.sh hostname

servers=(
    "root@server01.example.com"
    "root@server02.example.com"
    "root@server03.example.com"
    )

    for server in ${servers[@]}; do
        ssh "$server" "$@"
    done
