# Copyright 2018-present Open Networking Foundation
#
# SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0

SHELL		:= /bin/bash
BUILD		?= /tmp/build
M		?= $(BUILD)/milestones
MAKEDIR		:= $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
SCRIPTDIR	:= $(MAKEDIR)/scripts
RESOURCEDIR	:= $(MAKEDIR)/resources
WORKSPACE	?= $(HOME)
VENV		?= $(BUILD)/venv/aiab

4G_CORE_VALUES ?= $(MAKEDIR)/4g-core-values.yaml
5G_CORE_VALUES ?= $(MAKEDIR)/5g-core-values.yaml
OAISIM_VALUES  ?= $(MAKEDIR)/oaisim-values.yaml
ROC_VALUES     ?= $(MAKEDIR)/roc-values-v4.yaml
UPF_VALUES     ?= $(MAKEDIR)/upf-values.yaml
RANSIM_VALUES  ?= $(MAKEDIR)/ransim-values.yaml
ROC_4G_MODELS  ?= $(MAKEDIR)/roc-4g-models-v4.json
ROC_5G_MODELS  ?= $(MAKEDIR)/roc-5g-models-v4.json
TEST_APP_VALUES?= $(MAKEDIR)/5g-test-apps-values.yaml

KUBESPRAY_VERSION ?= release-2.17
DOCKER_VERSION	?= '20.10'
K8S_VERSION	?= v1.20.11
HELM_VERSION	?= v3.6.3
ENABLE_SUBSCRIBER_PROXY ?= false
GNBSIM_COLORS ?= true

HELM_GLOBAL_ARGS ?=

# Allow installing local charts or specific versions of published charts.
# E.g., to install the Aether 1.5 release:
#    CHARTS=release-1.5 make test
# Default is to install from the local charts.
CHARTS     ?= local
CONFIGFILE := configs/$(CHARTS)
include $(CONFIGFILE)
include configs/authentication

cpu_family	:= $(shell lscpu | grep 'CPU family:' | awk '{print $$3}')
cpu_model	:= $(shell lscpu | grep 'Model:' | awk '{print $$2}')
os_vendor	:= $(shell lsb_release -i -s)
os_release	:= $(shell lsb_release -r -s)

omec: $(M)/system-check $(M)/omec
oaisim: $(M)/oaisim
5gc: $(M)/system-check $(M)/5g-core

.PHONY: omec oaisim 5gc test reset-test reset-ue 5g-core reset-5g-test clean

$(M):
	mkdir -p $(M)

$(M)/system-check: | $(M)
	@if [[ $(cpu_family) -eq 6 ]]; then \
		if [[ $(cpu_model) -lt 60 ]]; then \
			echo "FATAL: haswell CPU or newer is required."; \
			exit 1; \
		fi \
	else \
		echo "FATAL: unsupported CPU family."; \
		exit 1; \
	fi
	@if [[ $(os_vendor) =~ (Ubuntu) ]]; then \
		if [[ ! $(os_release) =~ (18.04) ]]; then \
			echo "WARN: $(os_vendor) $(os_release) has not been tested."; \
		fi; \
		if dpkg --compare-versions 4.15 gt $(shell uname -r); then \
			echo "FATAL: kernel 4.15 or later is required."; \
			echo "Please upgrade your kernel by running" \
			"apt install --install-recommends linux-generic-hwe-$(os_release)"; \
			exit 1; \
		fi \
	else \
		echo "FAIL: unsupported OS."; \
		exit 1; \
	fi
	touch $@

$(M)/setup: | $(M)
	sudo $(SCRIPTDIR)/cloudlab-disksetup.sh
	sudo apt update; sudo apt install -y software-properties-common python3 python3-pip python3-venv jq httpie ipvsadm
	touch $@

$(BUILD)/kubespray: | $(M)/setup
	mkdir -p $(BUILD)
	cd $(BUILD); git clone https://github.com/kubernetes-incubator/kubespray.git -b $(KUBESPRAY_VERSION)

$(VENV)/bin/activate: | $(M)/setup
	python3 -m venv $(VENV)
	source "$(VENV)/bin/activate" && \
	python -m pip install -U pip && \
	deactivate

$(M)/kubespray-requirements: $(BUILD)/kubespray | $(VENV)/bin/activate
	source "$(VENV)/bin/activate" && \
	pip install -r $(BUILD)/kubespray/requirements.txt
	touch $@

$(M)/k8s-ready: | $(M)/setup $(BUILD)/kubespray $(VENV)/bin/activate $(M)/kubespray-requirements
	source "$(VENV)/bin/activate" && cd $(BUILD)/kubespray; \
	ansible-playbook -b -i inventory/local/hosts.ini \
		-e "{'override_system_hostname' : False, 'disable_swap' : True}" \
		-e "{'docker_version' : $(DOCKER_VERSION)}" \
		-e "{'docker_iptables_enabled' : True}" \
		-e "{'kube_version' : $(K8S_VERSION)}" \
		-e "{'kube_network_plugin_multus' : True, 'multus_version' : stable, 'multus_cni_version' : 0.3.1}" \
		-e "{'kube_proxy_metrics_bind_address' : 0.0.0.0:10249}" \
		-e "{'kube_pods_subnet' : 192.168.0.0/17, 'kube_service_addresses' : 192.168.128.0/17}" \
		-e "{'kube_apiserver_node_port_range' : 2000-36767}" \
		-e "{'kubeadm_enabled': True}" \
		-e "{'kube_feature_gates' : [SCTPSupport=True]}" \
		-e "{'kubelet_custom_flags' : [--allowed-unsafe-sysctls=net.*]}" \
		-e "{'dns_min_replicas' : 1}" \
		-e "{'helm_enabled' : True, 'helm_version' : $(HELM_VERSION)}" \
		cluster.yml
	mkdir -p $(HOME)/.kube
	sudo cp -f /etc/kubernetes/admin.conf $(HOME)/.kube/config
	sudo chown $(shell id -u):$(shell id -g) $(HOME)/.kube/config
	kubectl wait pod -n kube-system --for=condition=Ready --all
	touch $@

$(M)/helm-ready: | $(M)/k8s-ready
	helm repo add incubator https://charts.helm.sh/incubator
	helm repo add cord https://charts.opencord.org
	helm repo add atomix https://charts.atomix.io
	helm repo add onosproject https://charts.onosproject.org
	@if [ "$(REPO_PASSWORD)" ]; then \
		helm repo add aether --username ${REPO_USERNAME} --password ${REPO_PASSWORD} https://charts.aetherproject.org; \
	fi
	touch $@

/opt/cni/bin/simpleovs: | $(M)/k8s-ready
	sudo cp $(RESOURCEDIR)/simpleovs /opt/cni/bin/

/opt/cni/bin/static: | $(M)/k8s-ready
	mkdir -p $(BUILD)/cni-plugins; cd $(BUILD)/cni-plugins; \
	wget https://github.com/containernetworking/plugins/releases/download/v0.8.2/cni-plugins-linux-amd64-v0.8.2.tgz && \
	tar xvfz cni-plugins-linux-amd64-v0.8.2.tgz
	sudo cp $(BUILD)/cni-plugins/static /opt/cni/bin/

# TODO: need to connect ONOS
$(M)/fabric: | $(M)/setup /opt/cni/bin/simpleovs /opt/cni/bin/static
	sudo apt install -y openvswitch-switch
	sudo ovs-vsctl --may-exist add-br br-enb-net
	sudo ovs-vsctl --may-exist add-port br-enb-net enb -- set Interface enb type=internal
	sudo ip addr add 192.168.251.4/24 dev enb || true
	sudo ip link set enb up
	sudo ethtool --offload enb tx off
	sudo ip route replace 192.168.252.0/24 via 192.168.251.1 dev enb
	kubectl apply -f $(RESOURCEDIR)/router.yaml
	kubectl wait pod -n default --for=condition=Ready -l app=router --timeout=300s
	kubectl -n default exec router -- ip route add 172.250.0.0/16 via 192.168.250.3
	kubectl delete net-attach-def core-net
	touch $@

auth-secret: $(RESOURCEDIR)/aether.registry.yaml
$(RESOURCEDIR)/aether.registry.yaml: configs/authentication
	@kubectl -n omec create secret docker-registry aether.registry \
		--docker-server=https://registry.aetherproject.org \
		--docker-username=${REGISTRY_USERNAME} \
		--docker-password=${REGISTRY_CLI_SECRET} \
		--dry-run=client --output=yaml > $@

$(M)/omec: | $(M)/helm-ready /opt/cni/bin/simpleovs /opt/cni/bin/static $(M)/fabric $(RESOURCEDIR)/aether.registry.yaml
	kubectl get namespace omec 2> /dev/null || kubectl create namespace omec
	kubectl -n omec get secret aether.registry || kubectl create -f $(RESOURCEDIR)/aether.registry.yaml
	helm repo update
	if [[ "${CHARTS}" == "local" || "${CHARTS}" == "local-sdcore" ]]; then helm dep up $(OMEC_CONTROL_PLANE_CHART); fi
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(4G_CORE_VALUES) \
		sim-app \
		$(OMEC_SUB_PROVISION_CHART) && \
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(4G_CORE_VALUES) \
		omec-control-plane \
		$(OMEC_CONTROL_PLANE_CHART) && \
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(UPF_VALUES) \
		omec-user-plane \
		$(OMEC_USER_PLANE_CHART)
	touch $@

$(M)/5g-core: | $(M)/helm-ready /opt/cni/bin/simpleovs /opt/cni/bin/static $(M)/fabric $(RESOURCEDIR)/aether.registry.yaml
	kubectl get namespace omec 2> /dev/null || kubectl create namespace omec
	kubectl -n omec get secret aether.registry || kubectl create -f $(RESOURCEDIR)/aether.registry.yaml
	helm repo update
	if [[ "${CHARTS}" == "local" || "${CHARTS}" == "local-sdcore" ]]; then helm dep up $(5GC_CONTROL_PLANE_CHART); fi
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(5G_CORE_VALUES) \
		sim-app \
		$(OMEC_SUB_PROVISION_CHART) && \
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(UPF_VALUES) \
		5g-core-up \
		$(OMEC_USER_PLANE_CHART) && \
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(5G_CORE_VALUES) \
		fgc-core \
		$(5GC_CONTROL_PLANE_CHART) && \
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		--values $(RANSIM_VALUES) \
		5g-ransim-plane \
		$(5G_RAN_SIM_CHART)
	touch $@

# UE images includes kernel module, ue_ip.ko
# which should be built in the exactly same kernel version of the host machine
$(BUILD)/openairinterface: | $(M)/setup
	mkdir -p $(BUILD)
	cd $(BUILD); git clone https://github.com/opencord/openairinterface.git

download-ue-image: | $(M)/k8s-ready
	sudo docker pull ${OAISIM_UE_IMAGE}
	sudo docker tag ${OAISIM_UE_IMAGE} omecproject/lte-uesoftmodem:1.1.0
	touch $(M)/ue-image

$(M)/ue-image: | $(M)/k8s-ready $(BUILD)/openairinterface
	cd $(BUILD)/openairinterface; \
	sudo docker build . --target lte-uesoftmodem \
		--build-arg build_base=omecproject/oai-base:1.1.0 \
		--file Dockerfile.ue \
		--tag omecproject/lte-uesoftmodem:1.1.0
	touch $@

$(M)/oaisim: | $(M)/ue-image $(M)/omec
	sudo ip addr add 127.0.0.2/8 dev lo || true
	$(eval mme_iface=$(shell ip -4 route list default | awk -F 'dev' '{ print $$2; exit }' | awk '{ print $$1 }'))
	helm upgrade --install $(HELM_GLOBAL_ARGS) --namespace omec oaisim cord/oaisim -f $(OAISIM_VALUES) \
		--set config.enb.networks.s1_mme.interface=$(mme_iface) \
		--set images.pullPolicy=IfNotPresent
	kubectl rollout status -n omec statefulset ue
	@timeout 60s bash -c \
	"until ip addr show oip1 | grep -q inet; \
	do \
		echo 'Waiting for UE 1 gets IP address'; \
		sleep 3; \
	done"
	touch $@

roc: $(M)/roc
$(M)/roc: $(M)/helm-ready
	kubectl get namespace aether-roc 2> /dev/null || kubectl create namespace aether-roc
	helm repo update
	if [ "$(CHARTS)" == "local" ]; then helm dep up $(AETHER_ROC_UMBRELLA_CHART); fi
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace kube-system \
		--values $(ROC_VALUES) \
		atomix-controller \
		$(ATOMIX_CONTROLLER_CHART)
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace kube-system \
		--values $(ROC_VALUES) \
		atomix-raft-storage \
		$(ATOMIX_RAFT_STORAGE_CHART)
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace kube-system \
		--values $(ROC_VALUES) \
		onos-operator \
		$(ONOS_OPERATOR_CHART)
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace aether-roc \
		--values $(ROC_VALUES) \
		aether-roc-umbrella \
		$(AETHER_ROC_UMBRELLA_CHART)
	touch $@

# Load the ROC 4G models.  Disable loading network slice from SimApp.
roc-4g-models: $(M)/roc
	sed -i 's/provision-network-slice: true/provision-network-slice: false/' $(4G_CORE_VALUES)
	sed -i 's/# syncUrl/syncUrl/' $(4G_CORE_VALUES)
	if [ "${ENABLE_SUBSCRIBER_PROXY}" == "true" ] ; then \
		sed -i 's/addr: config4g/addr: subscriber-proxy.aether-roc.svc.cluster.local/' $(4G_CORE_VALUES) ; \
	fi
	$(eval ONOS_CLI_POD := $(shell kubectl -n aether-roc get pods -l name=onos-cli -o name))
	echo "ONOS CLI pod: ${ONOS_CLI_POD}"
	until kubectl -n aether-roc exec ${ONOS_CLI_POD} -- \
		curl -s -f -L -X PATCH "http://aether-roc-api:8181/aether-roc-api" \
		--header 'Content-Type: application/json' \
		--data-raw "$$(cat ${ROC_4G_MODELS})"; do sleep 5; done

# Load the ROC 5G models.  Disable loading network slice from SimApp.
roc-5g-models: $(M)/roc
	sed -i 's/provision-network-slice: true/provision-network-slice: false/' $(5G_CORE_VALUES)
	sed -i 's/# syncUrl/syncUrl/' $(5G_CORE_VALUES)
	if [ "${ENABLE_SUBSCRIBER_PROXY}" == "true" ] ; then \
		sed -i 's/addr: webui/addr: subscriber-proxy.aether-roc.svc.cluster.local/' $(5G_CORE_VALUES) ;\
	fi
	$(eval ONOS_CLI_POD := $(shell kubectl -n aether-roc get pods -l name=onos-cli -o name))
	echo "ONOS CLI pod: ${ONOS_CLI_POD}"
	until kubectl -n aether-roc exec ${ONOS_CLI_POD} -- \
		curl -s -f -L -X PATCH "http://aether-roc-api:8181/aether-roc-api" \
		--header 'Content-Type: application/json' \
		--data-raw "$$(cat ${ROC_5G_MODELS})"; do sleep 5; done

roc-clean:
	@echo "This could take 2-3 minutes..."
	sed -i 's/provision-network-slice: false/provision-network-slice: true/' $(4G_CORE_VALUES)
	sed -i 's/  syncUrl/  # syncUrl/' $(4G_CORE_VALUES)
	sed -i 's/subscriber-proxy.aether-roc.svc.cluster.local/config4g/' $(4G_CORE_VALUES)
	sed -i 's/provision-network-slice: false/provision-network-slice: true/' $(5G_CORE_VALUES)
	sed -i 's/  syncUrl/  # syncUrl/' $(5G_CORE_VALUES)
	sed -i 's/subscriber-proxy.aether-roc.svc.cluster.local/webui/' $(5G_CORE_VALUES)
	kubectl delete namespace aether-roc || true
	rm -rf $(M)/roc

test: | $(M)/fabric $(M)/omec $(M)/oaisim
	@sleep 5
	@echo "Test1: ping from UE to SGI network gateway"
	ping -I oip1 192.168.250.1 -c 15
	@echo "Test2: ping from UE to 8.8.8.8"
	ping -I oip1 8.8.8.8 -c 3
	@echo "Test3: ping from UE to google.com"
	ping -I oip1 google.com -c 3
	@echo "Finished to test"

5g-test: | $(M)/5g-core
	@echo "Test: Registration + UE initiated PDU Session Establishment + User Data packets"
	@sleep 5
	@rm -f /tmp/gnbsim.out
	@if [[ ${GNBSIM_COLORS} == "true" ]]; then \
		kubectl -n omec exec gnbsim-0 -- ./gnbsim 2>&1 | tee /tmp/gnbsim.out; \
	else \
		kubectl -n omec exec gnbsim-0 -- ./gnbsim 2>&1 | sed -u "s,\x1B\[[0-9;]*[a-zA-Z],,g" | tee /tmp/gnbsim.out; \
	fi
	@echo ""
	@echo "Test summary:"
	@grep "Result: " /tmp/gnbsim.out
	@[ "$$(grep -c "Result: PASS" /tmp/gnbsim.out)" == "5" ] \
		&& echo "*** TEST PASSED ***" \
		|| (echo "*** TEST FAILED ***" && exit 1)

cleanup-omec:
	helm delete -n omec $$(helm -n omec ls -qa) || true
	@echo ""
	@echo "Wait for all pods to terminate..."
	kubectl wait -n omec --for=delete --all=true -l app!=ue pod --timeout=180s || true

reset-test: cleanup-omec
	kubectl delete po router || true
	cd $(M); rm -f oaisim omec fabric

reset-ue:
	helm delete -n omec oaisim || true
	kubectl wait -n omec --for=delete pod enb-0 || true
	kubectl wait -n omec --for=delete pod ue-0 || true
	cd $(M); rm -f oaisim

reset-5g-test: cleanup-omec
	cd $(M); rm -f 5g-core

reset-dbtestapp:
	helm uninstall --namespace omec 5g-test-app

dbtestapp:
	helm repo update
	if [ "$(CHARTS)" == "local" ]; then helm dep up $(5G_TEST_APPS_CHART); fi
	helm upgrade --install --wait $(HELM_GLOBAL_ARGS) \
		--namespace omec \
		5g-test-app \
		--values $(TEST_APP_VALUES) \
		$(5G_TEST_APPS_CHART)
	@echo "Finished to dbtestapp"

clean:
	kubectl delete po router || true
	kubectl delete net-attach-def core-net || true
	sudo ovs-vsctl del-br br-access-net || true
	sudo ovs-vsctl del-br br-core-net || true
	sudo apt remove --purge openvswitch-switch -y
	source "$(VENV)/bin/activate" && cd $(BUILD)/kubespray; \
	ansible-playbook -b -i inventory/local/hosts.ini reset.yml --extra-vars "reset_confirmation=yes"
	@if [ -d /usr/local/etc/emulab ]; then \
		mount | grep /mnt/extra/kubelet/pods | cut -d" " -f3 | sudo xargs umount; \
		sudo rm -rf /mnt/extra/kubelet; \
	fi
	rm -rf $(M)
