#!/bin/sh

. ~/rpki-auto-t1/rpki-auto-t1-env.sh

#for i in  1..$MAXNODES; do
#		ssh -t -t -o StrictHostKeyChecking=no child$i.$SUFFIX 'mkdir child$i; cd child$i; python /users/kyehwanl/root_rpki/subvert-rpki.hactrn.net/rpkid/myrpki.py -c /users/kyehwanl/rpki-auto-t1/rpki_conf/child$1/myrpki.conf'

#done

# ------ Making directory and configuration file(myrpki.conf) including roa, prefix files ---------
#for i in $(seq 1 20); do

var=$1
: ${var:=nothing}
if [ "$var" = "nothing" ]; then
	echo "enter number...program terminated"
else 
	ssh -t -t -o StrictHostKeyChecking=no child$var.$SUFFIX "cd rpki-auto-t1; mkdir child$var; cd child$var; pwd"
	sed -e "s/ImageHandle/child$var/g" -e "s/parent1.rpki-auto-t1/child$var.rpki-auto-t1/g" /users/kyehwanl/rpki-auto-t1/myrpki.conf > /users/kyehwanl/rpki-auto-t1/child$var/myrpki.conf
	node_name=`ssh child$var.$SUFFIX uname -n`
	: ${RPKI_NODE:=${node_name%%.rpki*}}
	echo "node name:$node_name , RPKI_NODE: $RPKI_NODE"

	id_tmp="`grep handle $RPKI_DIR/child$var/myrpki.conf | grep -v myrpki`"
	id="${id_tmp#*=?}"
	echo "RPKI system handle ID: $id"
	echo
	#ssh -t -t -o StrictHostKeyChecking=no child$i.$SUFFIX "cd $RPKI_DIR/child$i; python /users/kyehwanl/root_rpki/subvert-rpki.hactrn.net/rpkid/myrpki.py -c $RPKI_DIR/child$i/myrpki.conf"

	ssh -t -t -o StrictHostKeyChecking=no child$var.$SUFFIX "/users/kyehwanl/auto_rpki.sh"
fi
