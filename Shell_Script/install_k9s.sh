#!/bin/bash -x

##  Environment
# export TERM=xterm-256color
# Kubectl edit command will use this env var.
#  export KUBE_EDITOR=my_fav_editor

# install k9s
git clone https://github.com/derailed/k9s.git
cd k9s
make build
sudo cp -rf ./execs/k9s /usr/local/bin
./execs/k9s
