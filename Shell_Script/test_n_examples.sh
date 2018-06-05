#!/bin/sh

export SUFFIX="rpki-auto-t1.RPKI.emulab.antd.nist.gov"
export RPKI_DIR=/users/kyehwanl/rpki-auto-t1

function pause()
{
	echo
	echo " $@  ... Press Enter Key to Continue ..."
	read
	echo
}

function add_one()
{
	x=$1
	x=$(expr $x + 1)
	#echo x=$x
	return $x
}

pause " running start servers test ..."
function on_exit()
{
		#rm -f $temp_file && killall python
		rm -f $temp_file
		echo "program terminated normally"
}
trap on_exit EXIT
RPKI_SRC_DIR=/users/kyehwanl/Download/subvert-rpki.hactrn.net/rpkid
RPKI_DIR=/users/kyehwanl/rpki-auto-t1/child8
	cd $RPKI_DIR
	for i in "$(python $RPKI_SRC_DIR/start-servers.py -d | grep pid)"; do
		if [ -n "`echo $i | grep irdbd`" ]; then
			pid_tmp=${i#*pid?}
			pid_irdbd=${pid_tmp%%Started*}
			echo "irdbd pid: $pid_irdbd"
		fi

		if [ -n "`echo $i | grep rpkid`" ]; then
			pid_tmp=${pid_tmp#*pid?}
			pid_rpkid=${pid_tmp%%Started*}
			echo "rpkid pid: $pid_rpkid"
		fi

		if [ -n "`echo $i | grep pubd`" ]; then
			pid_tmp=${pid_tmp#*pid?}
			pid_pubd=${pid_tmp%Started*}
			echo "pubd pid: $pid_pubd"
		fi

		if [ -n "`echo $i | grep rootd`" ]; then
			pid_rootd=${pid_tmp##*pid?}
			echo "rootd pid: $pid_rootd"
		fi

		#echo -e "$i --> $pid"
	done
	exit 0


pause "running daemon test"
if [ -n "$(sudo netstat -tapn | grep python)" ]; then
	echo "python daemon running"
else
	echo "null"
fi

pause "mysql test"
P=root
U=root
D=rpkid
mysql -u$U -p$P <<< 'alter database '$D' default character set utf8;'
val=$(mysql -u$U -p$P -D$D <<< 'select child_handle from child;')
if [ -n "$val" ]; then
	echo $val
else
	echo "null"
fi


#------------------------
pause "number test"
#------------------------

y=1; add_one $y;
z=$?; echo "y=$y z=$z"

t=$(expr $y + 1)
echo " t = $t"

pause "number test end"

#---------------------------
#for i in $(seq 1 20); do
for i in 1 2; do
	echo " testing $i times"
done

#---------------------------
for ((a=1; a <= 5 ; a++))
do
   echo "Welcome $a times."
done










for i in 1; do

	ssh -t -t -o StrictHostKeyChecking=no child$i.$SUFFIX "cd rpki-auto-t1; mkdir child$i; cd child$i; pwd"
	sed -e "s/ImageHandle/child$i/g" -e "s/parent1.rpki-auto-t1/child$i.rpki-auto-t1/g" /users/kyehwanl/rpki-auto-t1/myrpki.conf > /users/kyehwanl/rpki-auto-t1/child$i/myrpki.conf
	node_name=`ssh child$i.$SUFFIX uname -n`
	: ${RPKI_NODE:=${node_name%%.rpki*}}
	echo "node name:$node_name , RPKI_NODE: $RPKI_NODE"

	id_tmp="`grep handle $RPKI_DIR/child$i/myrpki.conf | grep -v myrpki`"
	id="${id_tmp#*=?}"
	echo "RPKI system handle ID: $id"
	echo
	#ssh -t -t -o StrictHostKeyChecking=no child$i.$SUFFIX "cd $RPKI_DIR/child$i; python /users/kyehwanl/root_rpki/subvert-rpki.hactrn.net/rpkid/myrpki.py -c $RPKI_DIR/child$i/myrpki.conf"

	#ssh -t -t -o StrictHostKeyChecking=no child$i.$SUFFIX "/users/kyehwanl/auto_rpki.sh"

done




# How to use 'case'
srx_server_node=10.0.30.101
case "$1" in
  node01 | Node01 ) 
    echo open 10.0.30.75 2605
    ;;
  node02 | Node02 )
    echo open 10.0.30.82 2605
    ;;
  node03 | Node03 )
    echo open 10.0.30.84 2605
    ;;
  node04 | Node04 )
    echo open 10.0.30.70 2605
    ;;
  node05 | Node05 )
    echo open 10.0.30.72 2605
    ;;
  node06 | Node06 )
    echo open 10.0.30.67 2605
    ;;
    * ) 
      echo open $1 2605
      $srx_server_node=node5
      ;;
esac
