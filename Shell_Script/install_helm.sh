#!/bin/bash
set -x

curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3
chmod 700 get_helm.sh
sudo DESIRED_VERSION="v3.6.3" ./get_helm.sh
