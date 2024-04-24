# contributed by Anusha Nalluri  
# https://jira.o-ran-sc.org/projects/RIC/issues/RIC-1053?filter=allissues
# Script for Installing Docker,Kubernetes and Helm


KUBEVERSION="1.28.1-1.1"
DOCKERVERSION="20.10.21-0ubuntu1~20.04.2"
HELMVERSION="3.14.2"
VM_IP="x.x.x.x"

wait_for_pods_running () {
  NS="$2"
  CMD="kubectl get pods --all-namespaces "
  if [ "$NS" != "all-namespaces" ]; then
    CMD="kubectl get pods -n $2 "
  fi
  KEYWORD="Running"
  if [ "$#" == "3" ]; then
    KEYWORD="${3}.*Running"
  fi

  CMD2="$CMD | grep \"$KEYWORD\" | wc -l"
  NUMPODS=$(eval "$CMD2")
  echo "waiting for $NUMPODS/$1 pods running in namespace [$NS] with keyword [$KEYWORD]"
  while [  $NUMPODS -lt $1 ]; do
    sleep 5
    NUMPODS=$(eval "$CMD2")
    echo "> waiting for $NUMPODS/$1 pods running in namespace [$NS] with keyword [$KEYWORD]"
  done
}

# Uninstalling existing Docker, Kubernetes
echo "Uninstalling Docker,Kubernetes"
apt-get -y remove docker.io
kubeadm reset -f
sudo apt-get -y purge kubeadm kubectl kubelet kubernetes-cni kube*
sudo apt-get -y autoremove
sudo rm -rf ~/.kube
apt-get -y autoremove

SWAPFILES=$(grep swap /etc/fstab | sed '/^[ \t]*#/ d' | sed 's/[\t ]/ /g' | tr -s " " | cut -f1 -d' ')
if [ ! -z $SWAPFILES ]; then
  for SWAPFILE in $SWAPFILES
  do
    if [ ! -z $SWAPFILE ]; then
      echo "disabling swap file $SWAPFILE"
      if [[ $SWAPFILE == UUID* ]]; then
        UUID=$(echo $SWAPFILE | cut -f2 -d'=')
        swapoff -U $UUID
      else
        swapoff $SWAPFILE
      fi
      sed -i "\%$SWAPFILE%d" /etc/fstab
    fi
  done
fi

# Installing Docker
echo "****************************************************************************************************************"
echo "						Installing Docker						"
echo "****************************************************************************************************************"
apt-get install -y --allow-downgrades --allow-change-held-packages --allow-unauthenticated --ignore-hold docker.io=${DOCKERVERSION}
cat > /etc/docker/daemon.json <<EOF
{
  "exec-opts": ["native.cgroupdriver=systemd"],
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "100m"
  },
  "storage-driver": "overlay2"
}
EOF
mkdir -p /etc/systemd/system/docker.service.d
systemctl enable docker.service
systemctl daemon-reload
systemctl restart docker

# Helm Installation
echo "****************************************************************************************************************"
echo "						Installing Helm							"
echo "****************************************************************************************************************"
wget https://get.helm.sh/helm-v${HELMVERSION}-linux-amd64.tar.gz
tar -xvf helm-v${HELMVERSION}-linux-amd64.tar.gz
mv linux-amd64/helm /usr/local/bin/helm
helm version
rm  helm-v${HELMVERSION}-linux-amd64.tar.gz


# Installing Kubernetes Packages
echo "***************************************************************************************************************"
echo "						Installing Kubernetes						"
echo "***************************************************************************************************************"

rm /etc/apt/keyrings/kubernetes-apt-keyring.gpg
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://pkgs.k8s.io/core:/stable:/v1.28/deb/Release.key | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg 
echo 'deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v1.28/deb/ /' | sudo tee /etc/apt/sources.list.d/kubernetes.list 
sudo apt update


# Installing Kubectl, Kubeadm and kubelet

sudo apt install -y kubeadm=${KUBEVERSION} kubelet=${KUBEVERSION} kubectl=${KUBEVERSION}
kubeadm init --apiserver-advertise-address=${VM_IP} --pod-network-cidr=10.244.0.0/16 --v=5

mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
export KUBECONFIG=/etc/kubernetes/admin.conf

kubectl taint nodes --all node-role.kubernetes.io/control-plane-
kubectl taint nodes --all node.kubernetes.io/not-ready-

kubectl get pods -A
kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml

wait_for_pods_running 1 kube-flannel
wait_for_pods_running 7 kube-system

echo "***************************************************************************************************************"

kubectl get pods -A

echo "***************************************************************************************************************"


