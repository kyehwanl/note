#!/bin/sh
set -x

# install go lang
# (https://go.dev/doc/install)
wget https://go.dev/dl/go1.19.5.linux-amd64.tar.gz
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.19.5.linux-amd64.tar.gz
export PATH=$PATH:/usr/local/go/bin
echo "export PATH=$PATH:/usr/local/go/bin" >> ~/.profile
go version


# Docker CE install 
## system update
sudo apt update -y
sudo apt-get -y install curl apt-transport-https ca-certificates software-properties-common


## Add the Docker Repositories
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
sudo apt update -y


## Install Docker on Ubuntu 18.04
sudo apt install -y docker-ce



# install kind client 
curl -Lo ./kind https://kind.sigs.k8s.io/dl/v0.17.0/kind-linux-amd64
chmod +x ./kind
sudo mv ./kind /usr/local/bin/kind


# install kind cluster & run
# (https://kind.sigs.k8s.io/)
go install sigs.k8s.io/kind@v0.17.0 && kind create cluster

