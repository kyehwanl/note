#!/bin/sh
servers=(
"server01.example.com"
"user01@server02.example.com"
"root@server03.example.com"
"QUIT"
)

PS3=" connect server ?"

select server in "${servers[@]}"; do
    if [[ -z $server ]]; then
        echo "please select the number in menu"
        continue
    fi

    if [[ $server == "QUIT" ]]; then
        echo " quit"
        exit 0
    fi

    echo "access to the $server"
    ssh "$server"
done
