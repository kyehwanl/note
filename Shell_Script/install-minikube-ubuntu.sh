#! /bin/sh 
set -x

# 1. Install Docker
sudo apt update -y
sudo apt upgrade -y


#2. Install Minikube dependencies
sudo apt install -y curl wget apt-transport-https


#3. Download Minikube Binary
wget https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
sudo cp minikube-linux-amd64 /usr/local/bin/minikube
sudo chmod +x /usr/local/bin/minikube
minikube version


#4. Install Kubectl utility
curl -LO https://storage.googleapis.com/kubernetes-release/release/`curl -s https://storage.googleapis.com/kubernetes-release/release/stable.txt`/bin/linux/amd64/kubectl
chmod +x kubectl
sudo mv kubectl /usr/local/bin/


#5. Install Docker
sudo apt install docker.io -y
sudo usermod -aG docker ${USER}


#6. Start the minikube
/usr/bin/newgrp docker <<EONG
echo "newgrp starts a subshell with the group you specified"
id
minikube start --driver=docker
EONG


#7. Start with a new group, Otherwise need to logout & login again
newgrp docker



