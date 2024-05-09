
# Modify RIC-PLT Path in RICPATH Variables
RICPATH="/root/ric-dep"
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


