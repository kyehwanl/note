#!/bin/bash -x

# Following commands extracted from ONF 'aether-in-a-box', make node-prep
sudo apt update; sudo apt install -y software-properties-common python3 python3-pip python3-venv jq httpie ipvsadm apparmor apparmor-utils
systemctl list-units --full -all | grep "docker.service" || sudo apt install -y docker.io
sudo adduser $USER docker || true
echo -n ""
sudo mkdir -p /etc/rancher/rke2/
[ -d /usr/local/etc/emulab ] && [ ! -e /var/lib/rancher ] && sudo ln -s /var/lib/rancher /mnt/extra/rancher || true  # that link gets deleted on cleanup
echo "cni: multus,calico" >> config.yaml
echo "cluster-cidr: 192.168.84.0/24" >> config.yaml
echo "service-cidr: 192.168.85.0/24" >> config.yaml
echo "kubelet-arg:" >> config.yaml
echo "- --allowed-unsafe-sysctls="net.*"" >> config.yaml
echo "- --node-ip="129.6.142.13"" >> config.yaml
echo "pause-image: k8s.gcr.io/pause:3.3" >> config.yaml
echo "kube-proxy-arg:" >> config.yaml
echo "- --metrics-bind-address="0.0.0.0:10249"" >> config.yaml
echo "- --proxy-mode="ipvs"" >> config.yaml
echo "kube-apiserver-arg:" >> config.yaml
echo "- --service-node-port-range="2000-36767"" >> config.yaml
sudo mv config.yaml /etc/rancher/rke2/
curl -sfL https://get.rke2.io | sudo INSTALL_RKE2_VERSION=v1.23.15+rke2r1 sh -
sudo systemctl enable rke2-server.service
sudo systemctl start rke2-server.service
sudo /var/lib/rancher/rke2/bin/kubectl --kubeconfig /etc/rancher/rke2/rke2.yaml wait nodes --for=condition=Ready --all --timeout=300s
sudo /var/lib/rancher/rke2/bin/kubectl --kubeconfig /etc/rancher/rke2/rke2.yaml wait deployment -n kube-system --for=condition=available --all --timeout=300s
curl -LO "https://dl.k8s.io/release/v1.23.15/bin/linux/amd64/kubectl"
sudo chmod +x kubectl
sudo mv kubectl /usr/local/bin/
kubectl version --client
mkdir -p $HOME/.kube
sudo cp /etc/rancher/rke2/rke2.yaml $HOME/.kube/config
#sudo chown -R 1001:1001 $HOME/.kube --> 1001 means first user, but doesn't work with vagrant
sudo chown -R $USER $HOME/.kube
curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3
chmod 700 get_helm.sh
sudo DESIRED_VERSION=v3.10.3 ./get_helm.sh
helm repo add incubator https://charts.helm.sh/incubator
#helm repo add cord https://charts.opencord.org
#helm repo add atomix https://charts.atomix.io
#helm repo add onosproject https://charts.onosproject.org
#helm repo add aether https://charts.aetherproject.org
helm repo add rancher http://charts.rancher.io/
mkdir -p $HOME/.kube/cni-plugins; cd $HOME/.kube/cni-plugins; \
wget https://github.com/containernetworking/plugins/releases/download/v0.8.2/cni-plugins-linux-amd64-v0.8.2.tgz && \
tar xvfz cni-plugins-linux-amd64-v0.8.2.tgz
sudo cp $HOME/.kube/cni-plugins/static /opt/cni/bin/
