#!/bin/bash -x
IP_ADDR=$1

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



