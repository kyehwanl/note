#!/usr/bin/expect -f

set timeout -1
spawn git push --force --all github_repo

expect "Username*"
send -- "kyehwanlee\r"
expect "Password*"
send -- "Lee@A218\r"
send "\r"
expect eof

