#!/bin/bash -x

# install go 1.22.2
wget https://go.dev/dl/go1.22.2.linux-amd64.tar.gz
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.22.2.linux-amd64.tar.gz
echo "PATH=$PATH:/usr/local/go/bin" >> ${HOME}/.bashrc
export PATH=$PATH:/usr/local/go/bin
go version



