#! /bin/bash -x

sudo apt install -y vagrant virtualbox
vboxmanage setproperty machinefolder /proj/BGPSec/vagrant-work/VirtualBoxVMs
# echo "export VAGRANT_HOME=/proj/BGPSec/vagrant-work/.vagrant.d/" >> ~/.bash_profile
#cd /proj/BGPSec/vagrant-work/vagrant-images/ && mkdir -p test1 && vagrant init bento/ubuntu-18.04
if [ $VAGRANT_HOME == "" ]; then
  echo env missing ..
  echo "export VAGRANT_HOME=/proj/BGPSec/vagrant-work/.vagrant.d/" >> ~/.bash_profile
  export VAGRANT_HOME=/proj/BGPSec/vagrant-work/.vagrant.d/
fi
#vagrant up


