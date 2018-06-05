#!/bin/sh

##########################################################################
# Automated python myrpki tool shell script
# Kyehwan Lee.
# 09.10.2010
#
# Parent Alone mode
# Root alone mode
# Child interactive mode with parent
#
#
##########################################################################
global_text="global"

# --------------  Variable Declaration -------------------
#  RPKI_DIR, commonddb should be adjusted depending on 
#  user's environments
#
LOG=/tmp/rpki_log_$$
node_name=`uname -n`
: ${RPKI_NODE:=${node_name%%.*}}
RPKI_DIR=/users/kyehwanl/rpki-auto-t1/$RPKI_NODE
RPKI_SRC_DIR=/users/kyehwanl/Download/subvert-rpki.hactrn.net/rpkid
#RPKI_SRC_DIR=/users/kyehwanl/root_rpki/subvert-rpki.hactrn.net/rpkid
echo "node_name = $node_name, RPKI_NODE => $RPKI_NODE, RPKI_DIR=$RPKI_DIR"
temp_file=/tmp/myrpki_$$
identity=$RPKI_DIR/entitydb/identity.xml
commondb=/users/kyehwanl/rpki-auto-t1/tmp
repositories=$RPKI_DIR/entitydb/repositories
running_enable="false"
#trap 'rm -f $temp_file && killall python' EXIT
#trap 'rm -f $temp_file' EXIT
function on_exit()
{
	if [ "running_enable" = "not_termination" ]; then
		:
	else
		#rm -f $temp_file && killall python
		rm -f $temp_file
		echo "program terminated normally"
		echo
		exit 0
	fi
}
trap on_exit EXIT

# -------------- Function Declaration ---------------------
function info()
{
	echo
	echo "##########################################################"
	echo -e " \033[1m$@\033[0m"
	echo "##########################################################"
	echo
}

# This function uses "Here Document" for passing input to a command from a shell script 
# This document allows a command to execute as though it were reading from a file or the 
# keyboard, whereas in fact it's getting input from the script
#
function python_imple()
{
	cd $RPKI_DIR
	#echo $@
	python $RPKI_SRC_DIR/myrpki.py -c $RPKI_DIR/myrpki.conf << !AutoStuff!
	$@
	exit

!AutoStuff!
}

function python_mysql_setup()
{
	cd $RPKI_DIR
	echo $@
	python $RPKI_SRC_DIR/sql-setup.py << !AutoStuff!
	ts1@B211
	$@
!AutoStuff!
}

#
# this function is used to wait for the specified file exists
#
function waitfor()
{
	echo

	#while true; do 
	until [ -f $1 ]; do
		if [ -f $1 ] ; then 
			echo "file $1 exists"

		else 
			echo "file $1 doesn't exist"
			sleep 1
		fi
	done 
}

function pause() 
{
	echo
	echo "--- \"$@\"... Press Enter key to continue ..."
	read
	echo
}

#
# this function is used for extracting directory including file name
#    for example, input source is like,
#    myrpki> Parent calls itself 'TS2', we call it 'TS2' Parent calls us 'TS1' 
#    Writing /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/entitydb/repositories/TS2.xml 
#    This is the "repository referral" file to send to the repository operator myrpki> 
#
#	 output is, /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/entitydb/repositories/TS2.xml
function python_extract()
{
	text="$1"
	#echo "- implementation text --> $text"
	tmp1="${text#*Writing?}"
	tmp2="${tmp1%xml*}"
	echo "${tmp2}xml"
}

#
# This function could try to find a file which is sent as a parameter into this function at
#	commondb directory.
# param1 is directory information(parent or child), param2 is the string which the user wants
# to find.
#
function find_file()
{
	var=$2
	: ${var:=nothing} 
	if [ "$var" = "nothing" ]; then
		:
		echo "var = $var" >> temp_file
	else
		#[ -f $commondb/$1/*${var}* ] && echo "\"$var\" exist" || echo "\"$var\" doesn't exist"
		for file in $(ls -Rt $commondb/$1 ); do #--- -t: everytime newer file input
			echo $file | grep $var && break;	#--- break;just only one time newer file printout
		done
	fi
}

#
# this function is used for arithmetic calcuation, return one-incremented value as a return value
# the one that wants to receive return value should use '$?', which you will recall is the exit 
# status of the previous command.
#
# usage:
# 	y=1; add_one $y; z=$?; echo "y=$y z=$z"
#
function add_one()
{
	x=$1
	x=$(expr $x + 1)
	#echo x=$x
	return $x
}

#
# this function is to make root directories when it is called by a certain function.
#
function make_root_directories()
{
	mkdir $commondb/root
	mkdir $commondb/root/publication
	mkdir $commondb/root/pubclients
	mkdir $commondb/root/repositories
}

#
# This function is used for running daemons which are necessary to run rpki daemons.
# Also this function greps and prints each pid of running daemon.
#
function running_daemons()
{
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
}

info usage description
echo " ----------------------------------------------- "
echo " usage - auto.sh <mode> <rpki_dir> "
echo 
echo " 	mode	: parent | child"
echo "  rpki_dir: rpki implementation directory"
echo " ----------------------------------------------- "


#--- Default Mode setting : child or parent  ---
mode=$1
info Mode: $mode 
: ${mode:=child}
echo "\$mode --> $mode"

#--- System Variable Configuration ---
tmp_rpki_dir=$2
: ${tmp_rpki_dir:=$RPKI_DIR}
RPKI_DIR=$tmp_rpki_dir
echo "\$RPKI_DIR--> $RPKI_DIR"

#--- Configure system ID ---
id_tmp="`grep handle $RPKI_DIR/myrpki.conf | grep -v myrpki`"
id="${id_tmp#*=?}"
info "RPKI system handle(id) --> $id"

#--- pause ---
echo -e "----- myrpki automation script ------"
pause "auto script now starting"

# --------------------TEST MYSQL ------------------------
#cd $RPKI_DIR
#exec 0<"echo $pw"
#python sql-setup.py  | exec 0</users/kyehwanl/testinput
#python sql-setup.py  << eof
#exec ts1@B211
#read x < /users/kyehwanl/testinput
#eof
#pause "temp temp temp"
#python sql-setup.py  
#read x < /users/kyehwanl/testinput 
#pause "temp temp 2222"

# ---------------- making directory child and parent  ---------
if [ -d $commondb ] && [ -d $commondb/child ] && [ -d $commondb/child/pubclients ] && [ -d $commondb/child/repositories ] ; then
	true
else 
	mkdir $commondb
	mkdir $commondb/child
	mkdir $commondb/child/pubclients
	mkdir $commondb/child/repositories
fi

if [ -d $commondb ] && [ -d $commondb/parent ] && [ -d $commondb/parent/pubclients ] && [ -d $commondb/parent/repositories ]; then
	:
else 
	mkdir $commondb
	mkdir $commondb/parent
	mkdir $commondb/parent/pubclients
	mkdir $commondb/parent/repositories
fi

# ------------------ Child implementation ----------------------
info Mode: $mode

while :; do

	if [ "$mode" = "child" ]; then
		#--- Running Daemons - checking whether daemons are already running or not
		if [ -f $RPKI_DIR/bpki/servers/ca.cer ]; then
			if [ $running_enable = "false" ] && [ -z "$(sudo netstat -tapn | grep python)" ]; then
				running_daemons
				running_enable="true"
				pause "rpki is now running as a child mode..."
			else
				echo " python is already running ..."
			fi
		else
			#--- <initialize> ---
			pause "step1($mode): initialize "
			info ${identity##*/} >> temp_file
			result="$(python_imple initialize)"
			file_init="$(python_extract "$result")" 
			info "result:\n$result \n file_init: $file_init"
			waitfor $file_init

			#--- copy identity.xml to the parent directory
			pause "step2($mode): copy identity file to the parent directory"
			cp -f $file_init $commondb/parent/${id}_${file_init##*/}
			echo "copy $file_init $commondb/parent/${id}_${file_init##*/}"

			#--- wait for receiving response xml from parent
			pause "step3($mode): wait for response... "
			waitfor $commondb/child/${id}.xml
			info "found python_imple configure_parent $commondb/child/${id}.xml"

			#--- retrieve script from function call <configure_parent> ---
			#
			pause "step4($mode): configure_parent "
			result="$(python_imple configure_parent $commondb/child/${id}.xml)"
			file_loc="$(python_extract "$result")"
			info "result:\n$result \n file_loc: $file_loc"

			#--- according to output message(offer or referral), the flag set will be different ---
			#
			echo -e -n $result | grep referral && result2="referral" || result2="offer"

			#--- parameter expansion and matching processing - rip off and extract [test] ---
			#
			outfile="`echo ${result##*/}`"
			echo result :  ${outfile%?This*} >> temp_file
			outfile="${outfile%?This*}"
			echo outfile: $outfile >> temp_file

			dir_tmp="${result#*/}"
			echo "dir temp --> $dir_tmp" >> temp_file
			dir="/${dir_tmp%?This*}"
			echo "dir --> $dir"	>> temp_file

			#--- implementation at each case ---
			#
			if [ "$result2" = "referral" ] ; then
				pause "step5($mode): configure_publication_client "
				cp -f $file_loc $commondb/child/repositories
				echo "copy $file_loc $commondb/child/repositories"
				result="$(python_imple configure_publication_client $commondb/child/repositories/${file_loc##*/})"
				file_loc="$(python_extract "$result")"
				info " result:\n$result \n file_loc: $file_loc"

				pause "step6($mode): configure_repository"
				cp -f $file_loc $commondb/child/pubclients
				echo "copy $file_loc $commondb/child/pubclients"
				result="$(python_imple configure_repository $commondb/child/pubclients/${file_loc##*/})"
				file_loc="$(python_extract "$result")"
				info "result:\n$result \n"


			elif [ "$result2" = "offer" ] ; then
				pause "step5($mode): configure_publication_client "
				cp -f $file_loc $commondb/child/repositories
				result="$(python_imple configure_publication_client $commondb/child/repositories/${file_loc##*/})"
				file_loc="$(python_extract "$result")"
				info " result:\n$result \n file_loc: $file_loc"

				pause "step6($mode): configure_repository"
				cp -f $file_loc $commondb/child/pubclients
				result="$(python_imple configure_repository $commondb/child/pubclients/${file_loc##*/})"
				file_loc="$(python_extract "$result")"
				info "result:\n$result \n"
			fi

			pause "step7($mode): creating mysql db"
			result="$(python_mysql_setup ts1@B211)" #-- password : root
			info "result:\n$result \n"

			# ------------------ Running daemons and rip off daemon pid ----------------------
			pause "step8($mode): running daemons"
			#
			#for i in "$(sh -c "python start-servers.py -d" | grep pid)"; do
			#x=0; add_one $x; x=$?; echo $x
			running_daemons
			running_enable="true"
			echo "configure_daemons x 2times"
			sleep 1
			python_imple configure_daemons
			python_imple configure_daemons
			pause "rpki child done and running..."
			#running_enable="not_termination"
		fi
		break;

		#ff	if [ echo -e $result | grep referral ]; then
		#		echo "referral"
		#	fi
		#	echo

		#	info test_ result2
		#	result2 = echo -e -n $result | grep referral
		#	echo -e $result2

# ----------------- Parent implementation -----------------------------
#
	elif [ "$mode" = "parent" ]; then
		#--- initialize ---
		pause "step1($mode): initialize "
		info ${identity##*/} >> temp_file
		if [ -f $RPKI_DIR/entitydb/identity.xml ]; then
			echo -e "identity already exists"
		else
			result="$(python_imple initialize)"
			file_init="$(python_extract "$result")" #extract file information
			info "result:\n$result \n file_init: $file_init"
			waitfor $file_init
		fi
		#--- Running Daemons - checking whether daemons are already running or not
		if [ -f $RPKI_DIR/bpki/servers/ca.cer ]; then
			if [ $running_enable = "false" ] && [ -z "$(sudo netstat -tapn | grep python)" ]; then
				running_daemons
				running_enable="true"
			else
				echo " rpki daemons are already running "
			fi
		fi
		#--- configure child
		pause "step2($mode): configure_child"
		while :; do
			if [ -n "$(ls -R $commondb | grep identity)" ]; then
				id_file="$(find_file parent identity)"
				: ${saved_file:=nofile}
				echo "saved file=$saved_file, id_file=$id_file"
				if [ "$saved_file" = "$id_file" ]; then
					echo " waiting new input file..."
					sleep 10 
					continue
				else
					echo "identity file --> $id_file"
					result="$(python_imple configure_child $commondb/parent/$id_file)"
					file_init="$(python_extract "$result")" #extract file information
					info "id_file: $id_file \n result: $result \n file_init: ${file_init}"

					#--- copy identity.xml to the child directory
					pause "step3($mode): copy the response file to the child directory"
					cp -f $file_init $commondb/child/
					echo "copy $file_init $commondb/child/"
					info " parent setting done... (copy $file_init $commondb/child/)"
					saved_file=$id_file

					#--- insert the relationship into mysql 
					echo "configure_daemons "
					python_imple configure_daemons
					break;
				fi
			else
				:
				echo " waiting client input..."
				sleep 5
			fi
		done

		pause " in case of referral, copy parent's repository information to child"
		#cp -f $commondb/parent/pubclients/$id.xml $commondb/child/pubclients/$id.xml
		cp -f $RPKI_DIR/entitydb/pubclients/$id.xml $commondb/child/pubclients/$id.xml

# ----------------- Root implementation --------------------------------
#
	elif [ "$mode" = "root" ]; then
		#--- Checking Running Daemons - whether daemons are already running or not
		if [ -f $RPKI_DIR/bpki/servers/ca.cer ]; then
			if [ $running_enable = "false" ] && [ -z "$(sudo netstat -tapn | grep python)" ]; then
				running_daemons
				running_enable="true"
				pause "rpki is now running as a Root mode..."
			else
				echo "python is already running...goto parent mode"
				running_enable="true"
				mode=parent
				info " Mode Change -->$mode "
				pause "rpki running..."
			fi
		else
			make_root_directories
			#--- initialize ---
			pause "step1($mode): initialize "
			info ${identity##*/} >> temp_file
			if [ -f $RPKI_DIR/entitydb/identity.xml ]; then
				echo -e "identity already exists"
				result="$(python_imple initialize)"
				file_loc="$(python_extract "$result")" #extract file information
				if [ "$file_loc" = "myrpki> myrpki> xml" ]; then
					file_loc=$RPKI_DIR/entitydb/repositories/${id}.xml
				fi
				info "result:\n$result \n file_loc: $file_loc"
			else
				result="$(python_imple initialize)"
				file_loc="$(python_extract "$result")" #extract file information
				info "result:\n$result \n file_loc: $file_loc"
				waitfor $file_loc
			fi

			pause "step2($mode): configure_publication_client "
			result="$(python_imple configure_publication_client $file_loc)"
			file_loc="$(python_extract "$result")"
			if [ "${file_loc##*/}" = "None.xml" ]; then
				file_loc=$RPKI_DIR/entitydb/pubclients/${id}.xml
			fi
			info " result:\n$result \n file_loc: $file_loc"

			pause "step3($mode): configure_repository"
			result="$(python_imple configure_repository $file_loc)"
			file_loc="$(python_extract "$result")"
			info "result:\n$result \n"

			#cp -f $RPKI_DIR/rootd.cer $RPKI_DIR/publication/root.cer
			mkdir $RPKI_DIR/publication
			cp -f $RPKI_SRC_DIR/publication/root.cer $RPKI_DIR/publication/root.cer
			sudo rsync --daemon

			pause "step4($mode): creating mysql db"
			result="$(python_mysql_setup ts1@B211)" #-- password : root
			info "result:\n$result \n"

			pause "step5($mode): running daemons"
			#
			running_daemons
			running_enable="true"
			echo "configure_daemons x 2times"
			sleep 1
			python_imple configure_daemons
			python_imple configure_daemons
			pause "rpki root done and running..."
			mode=parent
			info " Mode Change -->$mode "
		fi	
	fi

done

exit 0






#var2=`mpls nhlfe add key 0 instructions push gen 500 nexthop eth2 ipv4 10.0.3.2 |grep key |cut -c 17-26`



#--- for reference --> myrpki configuration procedure  ---

#-- @TS2

# myrpki> configure_parent arin.xml 
# Parent calls itself 'arin', we call it 'arin'
# Parent calls us 'NIST'
# Writing /users/kyehwanl/subvert-rpki.hactrn.net/rpkid/entitydb/repositories/arin.xml
# This is the "repository offer" file to send to the repository operator

# myrpki> configure_publication_client entitydb/repositories/arin.xml 
# Don't know where to nest this client, defaulting to top-level
# Client calls itself 'TS2', we call it 'TS2'
# Client says its parent handle is 'arin'
# Writing /users/kyehwanl/subvert-rpki.hactrn.net/rpkid/entitydb/pubclients/TS2.xml
# Send this file back to the publication client you just configured

# myrpki> configure_repository entitydb/pubclients/TS2.xml 
# Repository calls us 'TS2'
# Repository response associated with parent_handle 'arin'

# myrpki> configure_child identity.xml 
# Child calls itself 'TS1', we call it 'TS1'
# Writing /users/kyehwanl/subvert-rpki.hactrn.net/rpkid/entitydb/children/TS1.xml
# Send this file back to the child you just configured


#-- @TS1

# myrpki> initialize 
# Generating 2048-bit RSA key /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/bpki/servers/rpkid.key
# Generating 2048-bit RSA key /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/bpki/servers/irdbd.key
# Writing /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/entitydb/identity.xml
# This is the "identity" file you will need to send to your parent

# myrpki> configure_parent TS1.xml 
# Parent calls itself 'TS2', we call it 'TS2'
# Parent calls us 'TS1'
# Writing /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/entitydb/repositories/TS2.xml
# This is the "repository referral" file to send to the repository operator

# myrpki> configure_publication_client entitydb/repositories/TS2.xml 
# Found <authorization/> element, this looks like a referral
# Client calls itself 'TS1', we call it 'TS2/TS1'
# Client says its parent handle is 'TS2'
# Writing /users/www/proj-iip-bgp/RPKI/subvert-rpki.hactrn.net/rpkid/entitydb/pubclients/TS2.TS1.xml
# Send this file back to the publication client you just configured

# myrpki> configure_repository entitydb/pubclients/TS2.TS1.xml 
# Repository calls us 'TS2/TS1'
# Repository response associated with parent_handle 'TS2'


# ts1.antd [56]{.../rpki_link/rpkid}-> python start-servers.py 
# Started 'irdbd', pid 25629
# Started 'rpkid', pid 25630
# Started 'pubd', pid 25631


## Loop through each arg and display
        #for (( i = 1; i <= $numArgs; i++ )); do
		#	:
		#done
