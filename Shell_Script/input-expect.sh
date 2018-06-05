#!/usr/bin/expect -f

set timeout -1
spawn git push --all origin 

expect "Username*"
send -- "kyehwanl\r"
expect "Password*"
send -- "lkhgit1\r"
send "\r"
expect eof

