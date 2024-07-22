#!/bin/bash -x
#KUBEV="1.29.1"
KUBEV=$2
IP_ADDR=$4
KUBECNIV="0.7.5"
HELMV="3.5.4"
DOCKERV="20.10.21"


set -x
export DEBIAN_FRONTEND=noninteractive
echo "$(hostname -I) $(hostname)" >> /etc/hosts
printenv

IPV6IF=""

rm -rf /opt/config
mkdir -p /opt/config
echo "" > /opt/config/docker_version.txt
echo "1.16.0" > /opt/config/k8s_version.txt
echo "0.7.5" > /opt/config/k8s_cni_version.txt
echo "3.5.4" > /opt/config/helm_version.txt
echo "$(hostname -I)" > /opt/config/host_private_ip_addr.txt
echo "$(curl ifconfig.co)" > /opt/config/k8s_mst_floating_ip_addr.txt
echo "$(hostname -I)" > /opt/config/k8s_mst_private_ip_addr.txt
echo "__mtu__" > /opt/config/mtu.txt
echo "__cinder_volume_id__" > /opt/config/cinder_volume_id.txt
echo "$(hostname)" > /opt/config/stack_name.txt

ISAUX='false'
if [[ $(cat /opt/config/stack_name.txt) == *aux* ]]; then
  ISAUX='true'
fi

modprobe -- ip_vs
modprobe -- ip_vs_rr
modprobe -- ip_vs_wrr
modprobe -- ip_vs_sh
modprobe -- nf_conntrack_ipv4
modprobe -- nf_conntrack_ipv6
modprobe -- nf_conntrack_proto_sctp

if [ ! -z "$IPV6IF" ]; then
  start_ipv6_if $IPV6IF
fi

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



sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
echo   "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg]  https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt update
sudo apt install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin -y
sudo apt install -y apt-transport-https ca-certificates curl

# N = 2 # max number of worker nodes
# VER = '1.29.1-1.1' # Kubernetes Version to install
# ARG = 'Main'

sudo apt install -y git vim bash-completion

# Bridged Network Traffic
modprobe br_netfilter
echo 1 > /proc/sys/net/bridge/bridge-nf-call-iptables
echo 1 > /proc/sys/net/ipv4/ip_forward


# local small dns & vagrant cannot parse and delivery shell code.
echo "$4 m-k8s" >> /etc/hosts
for (( i=1; i<=$1; i++  )); do echo "192.168.1.10$i w$i-k8s" >> /etc/hosts; done

# config DNS
#cat <<EOF > /etc/resolv.conf
#nameserver 1.1.1.1 #cloudflare DNS
#nameserver 8.8.8.8 #Google DNS
#EOF


# docker key configuration



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

sudo apt update 
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



# install kubernetes
#sudo apt install -y kubeadm=1.29.1-1.1 kubelet=1.29.1-1.1 kubectl=1.29.1-1.1
#sudo apt install -y kubeadm=$2 kubelet=$2 kubectl=$2
#sudo apt-mark hold kubelet kubeadm kubectl


# git clone _Book_k8sInfra.git 
if [ $3 = 'Main' ]; then
  git clone https://github.com/sysnet4admin/_Book_k8sInfra.git
  mv /home/vagrant/_Book_k8sInfra $HOME
  find $HOME/_Book_k8sInfra/ -regex ".*\.\(sh\)" -exec chmod 700 {} \;
fi

cat <<EOF >/root/config.yaml
apiVersion: kubeadm.k8s.io/v1beta3
kind: InitConfiguration
bootstrapTokens:
  - token: "123456.1234567890123456"
    description: "kubeadm bootstrap token"
    ttl: "0"
localAPIEndpoint:
  advertiseAddress: ${IP_ADDR}
  bindPort : 6443
---
apiVersion: kubeadm.k8s.io/v1beta3
kubernetesVersion: v1.29.1
kind: ClusterConfiguration
networking:
  dnsDomain: cluster.local
  podSubnet: 10.244.0.0/16
  serviceSubnet: 10.96.0.0/12
---
apiVersion: kubeproxy.config.k8s.io/v1alpha1
kind: KubeProxyConfiguration
mode: ipvs
EOF



cat <<EOF > /root/rbac-config.yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: tiller
  namespace: kube-system
---
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRoleBinding
metadata:
  name: tiller
roleRef:
  apiGroup: rbac.authorization.k8s.io
  kind: ClusterRole
  name: cluster-admin
subjects:
  - kind: ServiceAccount
    name: tiller
    namespace: kube-system
EOF

kubeadm init --config /root/config.yaml 

rm -rf $HOME/.kube
mkdir -p $HOME/.kube
cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
chown $(id -u):$(id -g) $HOME/.kube/config
export KUBECONFIG=$HOME/.kube/config
echo "KUBECONFIG=${KUBECONFIG}" >> /etc/environment

kubectl get pods --all-namespaces

### Pod Network (CNI)
# policy/v1beta1 PodSecurityPolicy in flannel, is deprecated in v1.21
kubectl apply -f https://github.com/flannel-io/flannel/releases/latest/download/kube-flannel.yml
#kubectl apply -f https://docs.projectcalico.org/manifests/calico.yaml
#kubectl apply -f https://raw.githubusercontent.com/sysnet4admin/IaC/master/manifests/172.16_net_calico.yaml
#kubectl apply -f https://github.com/weaveworks/weave/releases/download/v2.8.1/weave-daemonset-k8s.yaml


# master node is no longer used, instead control-plane 
# Here, taint prevent master from generating pods inside master node
#kubectl taint nodes --all node-role.kubernetes.io/control-plane-



