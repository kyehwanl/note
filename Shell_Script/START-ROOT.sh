#!/bin/sh

. ~/rpki-auto-t1/rpki-auto-t1-env.sh

# ------ Making directory and configuration file(myrpki.conf) including roa, prefix files ---------
var=$1
: ${var:=nothing}
if [ "$var" = "nothing" ]; then
	echo "enter [host]...program terminated"
else 
	ssh -t -t -o StrictHostKeyChecking=no $var.$SUFFIX "cd rpki-auto-t1; mkdir $var; cd $var; pwd"
	sed -e "s/ImageHandle/$var/g" -e "s/parent1.rpki-auto-t1.RPKI.emulab.antd.nist.gov/$var.$SUFFIX/g" -e  "s/run_rootd = false/run_rootd = true/g" /users/kyehwanl/rpki-auto-t1/myrpki.conf > /users/kyehwanl/rpki-auto-t1/$var/myrpki.conf
	node_name=`ssh parent$i.$SUFFIX uname -n`
	: ${RPKI_NODE:=${node_name%%.rpki*}}
	echo "node name:$node_name , RPKI_NODE: $RPKI_NODE"

	id_tmp="`grep handle $RPKI_DIR/parent$i/myrpki.conf | grep -v myrpki`"
	id="${id_tmp#*=?}"
	echo "RPKI system handle ID: $id"
	echo
	#ssh -t -t -o StrictHostKeyChecking=no parent$i.$SUFFIX "cd $RPKI_DIR/parent$i; python /users/kyehwanl/root_rpki/subvert-rpki.hactrn.net/rpkid/myrpki.py -c $RPKI_DIR/parent$i/myrpki.conf"

	ssh -t -t -o StrictHostKeyChecking=no parent$i.$SUFFIX "/users/kyehwanl/auto_rpki.sh root"
fi
