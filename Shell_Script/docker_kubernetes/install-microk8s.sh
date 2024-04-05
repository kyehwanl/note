#!/bin/bash -x

# copied from ORAN SMO Package
# https://gerrit.o-ran-sc.org/r/it/dep.git
# ./dep/smo-install/scripts/layer-0/0-setup-microk8s.sh 

## Microk8S part
sudo apt-get update
sudo swapoff -a
sudo sed -i '/ swap / s/^\(.*\)$/#\1/g' /etc/fstab
snap remove microk8s
snap remove kubectl
snap install microk8s --classic --channel=1.27/stable
sudo snap install kubectl --classic --channel=1.27/stable

## Firewall
ufw allow in on cni0 && sudo ufw allow out on cni0
ufw default allow routed

## Enable required features for K8S
microk8s enable dns storage prometheus

## Setup kubectl
cd
mkdir -p .kube
cd .kube
sudo microk8s.config > config
chmod 700 config

#Check the install
echo "Checking Kubernetes ..."
kubectl version

