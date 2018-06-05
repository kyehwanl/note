#!/bin/sh

. ~/rpki-auto-t1/rpki-auto-t1-env.sh


# ------ Making directory and configuration file(myrpki.conf) including roa, prefix files ---------

var=$1
mode=$2
: ${var:=nothing}
if [ "$var" = "nothing" ]; then
	echo "enter [host]...program terminated"
	echo
	exit
else 
	if [ "${var#*.}" = "$var" ]; then
		node_name=`ssh $var.$SUFFIX uname -n`
	else
		SUFFIX=${var#*.}
		var=${var%%.*}
		node_name=`ssh $var.$SUFFIX uname -n`
	fi
	echo  "current HOST_NAME:$HOST_NAME  target HOST:$var  SUFFIX:$SUFFIX"
	: ${RPKI_NODE:=${node_name%%.rpki*}}
	echo "target node name:$node_name , RPKI_NODE: $RPKI_NODE"
	if [ "$node_name" = "" ]; then
		echo Error: Name or service not known
		echo
		exit
	fi

	if [ "$mode" = "parent" ]; then
		ssh $var.$SUFFIX "/users/kyehwanl/auto_rpki.sh parent"
		echo "proceed parent mode..."
		exit
	fi


	ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "cd rpki-auto-t1; mkdir $var; cd $var; pwd"
	sed -e "s/\/users\/kyehwanl\/publication/\/users\/kyehwanl\/rpki-auto-t1\/$var\/publication/" rsyncd.conf > rsyncd.conf.new
	ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "cd rpki-auto-t1; sudo cp -u rsyncd.conf.new /etc/rsyncd.conf"
	ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "sudo rsync --daemon"
	#ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "cd rpki-auto-t1; cp -u *.csv $var"

	#if [ "$var" = "parent1" ] || [ "$var" = "parent2" ] || [ "$var" = "parent3" ] ; then
	if [ "$var" = "parent1" ] ; then
		sed -e "s/ImageHandle/$var/g" -e "s/parent1.rpki-auto-t1.RPKI.emulab.antd.nist.gov/$var.$SUFFIX/g" -e  "s/run_rootd = false/run_rootd = true/g" /users/kyehwanl/rpki-auto-t1/myrpki.conf > /users/kyehwanl/rpki-auto-t1/$var/myrpki.conf
		ssh $var.$SUFFIX "/users/kyehwanl/auto_rpki.sh root"
	else
		sed -e "s/ImageHandle/$var/g" -e "s/parent1.rpki-auto-t1.RPKI.emulab.antd.nist.gov/$var.$SUFFIX/g" /users/kyehwanl/rpki-auto-t1/myrpki.conf > /users/kyehwanl/rpki-auto-t1/$var/myrpki.conf
		#ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "/users/kyehwanl/auto_rpki.sh"
		ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "cd rpki-auto-t1/repository/$var; cp -u *.csv /users/kyehwanl/rpki-auto-t1/$var"
		ssh $var.$SUFFIX "/users/kyehwanl/auto_rpki.sh"
	fi
fi

exit 0
