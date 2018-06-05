#!/bin/sh
#usage: ./shift.sh one two three

while [[ "$@" != "" ]]; do
    echo "$@"
    shift 
done 
