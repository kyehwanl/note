#!/usr/bin/expect -f

set timeout -1
spawn git push --force --all origin 

expect "Username*"
send -- "kyehwanl\r"
expect "Password*"
send -- "lkhgit1\r"
send "\r"
expect eof

