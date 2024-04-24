# contributed by Anusha Nalluri
# https://jira.o-ran-sc.org/projects/RIC/issues/RIC-1053?filter=allissues
# Note: By default ric-dep installation have this kong pods, with this Installation Kong Controller is not deploying, so we are disabling kong and we are installing Standalone Kong Pods

# Modify RIC-PLT Path in RICPATH Variables

RICPATH="xxxx/ric-dep"
snap install yq

# Changing api version in ric-dep folder as v1 API Version is deprecated

cd ${RICPATH}/helm
find ${RICPATH}/helm -type f -print0 | xargs -0 sed -i 's\v1beta1\v1\g'

cd ${RICPATH}/depRicKubernetesOperator
find ${RICPATH}/depRicKubernetesOperator -type f -print0 | xargs -0 sed -i 's\v1beta1\v1\g'

cd ${RICPATH}/ric-common
find ${RICPATH}/ric-common -type f -print0 | xargs -0 sed -i 's\v1beta1\v1\g'


# Disabling kong pod

cd ${RICPATH}/helm/infrastructure/
yq '.kong.enabled = false' -i values.yaml
yq '.kong.enabled' values.yaml

# Removing Ingress files

cd ${RICPATH}/helm/appmgr/templates
rm ingress-appmgr.yaml

cd ${RICPATH}/helm/e2mgr/templates
rm ingress-e2mgr.yaml

cd ${RICPATH}/helm/a1mediator/templates
rm ingress-a1mediator.yaml



# Installing Kong standalone Pod
echo "****************************************************************************************************************"
echo "						Installing Kong							"
echo "****************************************************************************************************************"
kubectl create ns kong
helm repo add kong https://charts.konghq.com
helm repo update
helm install kong kong/ingress -n kong

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

wait_for_pods_running 2 kong
sleep	25
echo "****************************************************************************************************************"
kubectl get pods -n kong
echo "****************************************************************************************************************"

