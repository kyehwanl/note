#!/bin/sh

domain_name=`uname -n`
SUFFIX=${domain_name#*.}
HOST_NAME=${domain_name%%.*}

export SUFFIX
export HOST_NAME
export MAXNODES="10"
export RPKI_DIR=/users/kyehwanl/rpki-auto-t1


