#!/usr/bin/expect -f

set timeout -1
spawn git push --force --all origin 

expect "Username*"
send -- "kyehwanl\r"
expect "Password*"
send -- "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\r"
send "\r"
expect eof

