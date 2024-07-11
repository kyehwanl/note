#!/bin/bash 
#
################################################################################
#   Copyright (c) 2019 AT&T Intellectual Property.                             #
#   Copyright (c) 2022 Nokia.                                                  #
#                                                                              #
#   Licensed under the Apache License, Version 2.0 (the "License");            #
#   you may not use this file except in compliance with the License.           #
#   You may obtain a copy of the License at                                    #
#                                                                              #
#       http://www.apache.org/licenses/LICENSE-2.0                             #
#                                                                              #
#   Unless required by applicable law or agreed to in writing, software        #
#   distributed under the License is distributed on an "AS IS" BASIS,          #
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
#   See the License for the specific language governing permissions and        #
#   limitations under the License.                                             #
################################################################################




#KUBEV="1.16.0"
KUBEV="1.29.1"
KUBECNIV="0.7.5"
HELMV="3.5.4"
DOCKERV="20.10.21"

set -x


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


echo "### Docker version  = "${DOCKERV}
echo "### k8s version     = "${KUBEV}
echo "### helm version    = "${HELMV}
echo "### k8s cni version = "${KUBECNIV}

KUBEVERSION="${KUBEV}-1.1"
CNIVERSION="${KUBECNIV}-00"
DOCKERVERSION="${DOCKERV}"

UBUNTU_RELEASE=$(lsb_release -r | sed 's/^[a-zA-Z:\t ]\+//g')
if [[ ${UBUNTU_RELEASE} == 16.* ]]; then
  echo "Installing on Ubuntu $UBUNTU_RELEASE (Xenial Xerus) host"
  if [ ! -z "${DOCKERV}" ]; then
    DOCKERVERSION="${DOCKERV}-0ubuntu1~16.04.5"
  fi
elif [[ ${UBUNTU_RELEASE} == 18.* ]]; then
  echo "Installing on Ubuntu $UBUNTU_RELEASE (Bionic Beaver)"
  if [ ! -z "${DOCKERV}" ]; then
    DOCKERVERSION="${DOCKERV}-0ubuntu1~18.04.4"
  fi
elif [[ ${UBUNTU_RELEASE} == 20.* ]]; then
  echo "Installing on Ubuntu $UBUNTU_RELEASE (Focal Fossal)"
  if [ ! -z "${DOCKERV}" ]; then
    DOCKERVERSION="${DOCKERV}-0ubuntu1~20.04.2"  # 20.10.21-0ubuntu1~20.04.2
  fi
else
  echo "Unsupported Ubuntu release ($UBUNTU_RELEASE) detected.  Exit."
fi

echo "docker version to use = "${DOCKERVERSION}

#curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | apt-key add -
#echo 'deb http://apt.kubernetes.io/ kubernetes-xenial main' > /etc/apt/sources.list.d/kubernetes.list

mkdir -p /etc/apt/apt.conf.d
echo "APT::Acquire::Retries \"3\";" > /etc/apt/apt.conf.d/80-retries

apt-get update
RES=$(apt-get install -y  curl jq netcat make ipset moreutils 2>&1)
if [[ $RES == */var/lib/dpkg/lock* ]]; then
  echo "Fail to get dpkg lock.  Wait for any other package installation"
  echo "process to finish, then rerun this script"
  exit -1
fi

APTOPTS="--allow-downgrades --allow-change-held-packages --allow-unauthenticated --ignore-hold "





sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
echo   "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg]  https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt update
sudo apt install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin -y
sudo apt install -y apt-transport-https ca-certificates curl

sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://pkgs.k8s.io/core:/stable:/v1.29/deb/Release.key | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg
echo 'deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v1.29/deb/ /' | sudo tee /etc/apt/sources.list.d/kubernetes.list

# Fix Containerd so that it does not disable CRI for Kubernetes
sudo rm /etc/containerd/config.toml
cat <<EOF >/etc/containerd/config.toml
version = 2
[plugins]
  [plugins."io.containerd.grpc.v1.cri"]
   [plugins."io.containerd.grpc.v1.cri".containerd]
      [plugins."io.containerd.grpc.v1.cri".containerd.runtimes]
        [plugins."io.containerd.grpc.v1.cri".containerd.runtimes.runc]
          runtime_type = "io.containerd.runc.v2"
          [plugins."io.containerd.grpc.v1.cri".containerd.runtimes.runc.options]
            SystemdCgroup = true
EOF
sudo systemctl restart containerd


# Bridged Network Traffic
modprobe br_netfilter
echo 1 > /proc/sys/net/bridge/bridge-nf-call-iptables
echo 1 > /proc/sys/net/ipv4/ip_forward


sudo apt update 
#sudo apt install -y kubeadm=1.29.1-1.1 kubelet=1.29.1-1.1 kubectl=1.29.1-1.1
#sudo apt-mark hold kubelet kubeadm kubectl


#if [ -z ${DOCKERVERSION} ]; then
#  apt-get install -y $APTOPTS docker.io
#else
#  apt-get install -y $APTOPTS docker.io=${DOCKERVERSION}
#fi
#cat > /etc/docker/daemon.json <<EOF
#{
#  "exec-opts": ["native.cgroupdriver=systemd"],
#  "log-driver": "json-file",
#  "log-opts": {
#    "max-size": "100m"
#  },
#  "storage-driver": "overlay2"
#}
#EOF
#mkdir -p /etc/systemd/system/docker.service.d
#systemctl enable docker.service
#systemctl daemon-reload
#systemctl restart docker

if [ -z ${CNIVERSION} ]; then
  apt-get install -y $APTOPTS kubernetes-cni
else
  apt-get install -y $APTOPTS kubernetes-cni=${CNIVERSION}
fi

if [ -z ${KUBEVERSION} ]; then
  apt-get install -y $APTOPTS kubeadm kubelet kubectl
else
  apt-get install -y $APTOPTS kubeadm=${KUBEVERSION} kubelet=${KUBEVERSION} kubectl=${KUBEVERSION}
fi

apt-mark hold docker.io kubernetes-cni kubelet kubeadm kubectl


kubeadm config images pull --kubernetes-version=${KUBEV}




#kubeadm init --apiserver-advertise-address=192.168.10.123 --pod-network-cidr=10.244.0.0/16 --v=5
kubeadm init --pod-network-cidr=10.244.0.0/16 

cd /root
rm -rf .kube
mkdir -p .kube
cp -i /etc/kubernetes/admin.conf /root/.kube/config
chown root:root /root/.kube/config
export KUBECONFIG=/root/.kube/config
echo "KUBECONFIG=${KUBECONFIG}" >> /etc/environment





kubectl taint nodes --all node-role.kubernetes.io/control-plane-
kubectl apply -f https://github.com/flannel-io/flannel/releases/latest/download/kube-flannel.yml

wget https://get.helm.sh/helm-v3.14.2-linux-amd64.tar.gz
tar -zxvf helm-v3.14.2-linux-amd64.tar.gz
mv linux-amd64/helm /usr/local/bin/helm
helm version

exit

git clone "https://gerrit.o-ran-sc.org/r/ric-plt/ric-dep"
cd ric-dep/
find -type f -print0 | xargs -0 sed -i 's\v1beta1\v1\g'
cd bin/
./install_common_templates_to_helm.sh


############################################################
# Until this line, vagrant snapshot ran2 k82_ready is done
############################################################



# --------- 1. Need to diable Kong setting first  ------------
# install without kong enabled
# --> In values.yaml need to change kong enabled parameter as false under ric-dep/helm/infrastructure folder. 
#     Kong pod will be in crashloopbackoff if we enabled as true


# -------  2.  Replace serviceName, servicePort in v1beta1 to service.name, service.port.number in v1
# they will be found at ./ric-dep/helm/a1mediator/templates/ingress-a1mediator.yaml 
#./install -f  ../RECIPE_EXAMPLE/example_recipe_oran_i_release.yaml
# --> this command leads to the next actual helm install command
#
# helm install -f ../RECIPE_EXAMPLE/example_recipe_oran_i_release.yaml --namespace ricplt r4-infrastructure /root/ric-dep/bin/../helm/infrastructure



# --- 3. install Kong ---
#helm repo add kong https://charts.konghq.com
#helm repo update
## helm install kong kong/ingress -n ricplt <-- instead of this, use following command
#helm install -f ~/ric-dep/helm/infrastructure/subcharts/kong/values.yaml kong kong/ingress -n ricplt 
#





