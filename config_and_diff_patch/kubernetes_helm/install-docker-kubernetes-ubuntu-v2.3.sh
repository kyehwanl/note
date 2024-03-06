#! /bin/bash 
# source: https://cla9.tistory.com/92?category=814452
# Comment: 
#  -- easier than file A, 
#  -- easier to install docker, also no need to delete containerd related file
set -x

# 1. Install Docker
sudo apt update -y
sudo apt install docker.io -y

sudo swapoff -a
sudo sed -i '/ swap / s/^\(.*\)$/#\1/g' /etc/fstab



# 2. Install Kubernetes 
# written on the official web page

sudo apt-get update && sudo apt-get install -y apt-transport-https curl
curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
cat <<EOF | sudo tee /etc/apt/sources.list.d/kubernetes.list
deb https://apt.kubernetes.io/ kubernetes-xenial main
EOF
sudo apt-get update
sudo apt-get install -y kubelet=1.25.5-00 kubeadm kubectl
sudo apt-mark hold kubelet kubeadm kubectl



# 3. Initializing 
sudo kubeadm init --pod-network-cidr 192.168.0.0/16

# setting for client to communicate with kube API Server
mkdir -p $HOME/.kube
sudo cp -f /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config

# Overlay Netwokr installation for CNI (using Weave Net)
#kubectl apply -f "https://cloud.weave.works/k8s/net?k8s-version=$(kubectl version | base64 | tr -d '\n')"
kubectl apply -f https://docs.projectcalico.org/manifests/calico.yaml

# get rid of taints, which prohibit from running one cluster
kubectl taint nodes --all node-role.kubernetes.io/control-plane:NoSchedule-
kubectl taint nodes --all node-role.kubernetes.io/master:NoSchedule-

# 4. test
kubectl get nodes
kubectl get po -A -o wide 

# 5. bash completion for k8s
source /etc/profile.d/bash_completion.sh
source <(kubectl completion bash)

