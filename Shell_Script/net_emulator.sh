#!/bin/sh +x

# init variables
max_count=500
start_ipaddr=2
bridge=br1
dir=${PWD}


function execute()
{
# echo -ne " -> Press enter to execute command:\n\033[1m$@\033[0m"
# read
 if [[ $dry_run -ge 1 ]]; then
    echo -n "dry-run: "
    echo -e "\033[1m$@\033[0m"
    return
 fi
 echo -e "\n\033[1m$@\033[0m"
 echo -ne "\E[32m"
 #sh -c "$@"
 "$@"
 #echo -e "\033[0m -> done"
 echo -ne "\033[0m"
 echo
}
function info()
{
 echo
 echo "---------------------------------------------------------"
 echo -e " \033[1m$@\033[0m"
 echo "---------------------------------------------------------"
 echo
}
function usage()
{
    echo "usage: $0 <option switch> [value]"
    echo "  options: "
    echo -e "\t-c, --count      : <value> instance_count"
    echo -e "\t-s, --start      : <value> start_point"
    echo -e "\t-b, --bgpsec     : bgpsec enable"
    echo -e "\t-d, --dry-run    : dry run test"
    echo -e "\t-n, --ns-only    : generate networkname space nodes only"
    echo -e "\t-f, --conf-only  : generate bgp configuration only"
    echo -e "\t-p, --path       : <path>, directory path for bgpd "
    echo -e "\t-r, --remove     : <0|1>, to remove setting"
    echo -e "\t-x, --prefix     : <number>, the number of prefixes to be generated"
    echo -e "\t-h, --help       : help screen"
    echo
}

function removeSetting()
{
    if [[ $remove = 0 ]]; then
        usage; exit
    fi

    echo
    echo +++ remove all setting called
    #echo bridge instance: $bridge

    info Remove network Namespace nodes
    for i in $(ip netns | awk '{print $1}'); do
        echo "remove network node: $i"
        ip netns del $i
    done

    info Remove bridge interfaces
    for i in $(brctl show $bridge | grep veth | awk '{print $4}'); do
        echo "remove bridge interface: $i"
        brctl delif $bridge $i
        #echo "ip link del $i"
    done
    if [ "`brctl show | grep $bridge | awk '{print $1}'`" = "$bridge" ]; then
        ip link del $bridge
        brctl delbr $bridge > /dev/null 2>&1
        echo "remove bridge : $bridge"
    fi


    info remove bgpd process
    for i in $(pgrep bgpd); do
        echo "bgpd process: $i"
        kill -9 $i
    done

    echo
    exit
}

function baseHeader()
{
echo '
hostname node1
password z
!log file node1/bgpd.log

router bgp 60001
bgp router-id 10.1.1.1
'  > $file_recv

if [[ $bgpsec -ge 1 ]]; then
echo '
srx bgpsec ski 0 1 C30433FA1975FF193181458FB902B501EA9789DC
srx bgpsec active 0
srx connect localhost 17900
srx evaluation bgpsec
' >> $file_recv
fi
}


function baseTail()
{
echo '
!network 110.1.1.0/24

!
line vty
!
log stdout debugging
!debug bgp
!debug bgp fsm
debug bgp updates
!debug bgp keepalives
!debug bgp events
!debug bgp filters
' >> $file_recv
if [[ $bgpsec -ge 1 ]]; then
echo '
debug bgp bgpsec
debug bgp bgpsec detail
debug bgp bgpsec out
debug bgp bgpsec in
' >> $file_recv
fi
}


function nodeHeader()
{
nh_p1=$1
echo "
hostname node$((nh_p1+2))
password z

router bgp $((nh_p1+2+60000))
bgp router-id 10.1.1.$((nh_p1+2))
neighbor 10.1.1.1 remote-as 60001
" > $2
}

function nodeBodyTail()
{
if [[ $bgpsec -ge 1 ]]; then
echo "
srx bgpsec ski 0 1 45CAD0AC44F77EFAA94602E9984305215BF47DCD
srx bgpsec active 0
srx connect localhost 17900
srx evaluation bgpsec
neighbor 10.1.1.1 bgpsec both
" >> $1
fi

echo "
line vty
log stdout debugging
debug bgp
debug bgp fsm
debug bgp updates
debug bgp keepalives
debug bgp events
debug bgp filters
" >> $1

# only bgpsec flag enabled
if [[ $bgpsec -ge 1 ]]; then
echo "
debug bgp bgpsec
debug bgp bgpsec detail
debug bgp bgpsec out
debug bgp bgpsec in
" >> $1
fi
}


function generatePrefix()
{
    echo "+++ generate prefix"
    #echo "arg1(num): $1, arg2(prefix): $2,  arg3(file): $3,  arg4(dry-run): $4"

    targetFile=$3
    #echo $targetFile

    for ((n=0; n<$1; n++)); do
        echo network $2.$n.0/24
        echo network $2.$n.0/24 >> $targetFile
    done
}

function configure_bridge()
{
    info bridge setting
    execute brctl addbr $bridge
    execute brctl stp $bridge off
    execute ifconfig $bridge 10.1.1.1/24 up
}



# Use -gt 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use -gt 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it such
# as in the --default example).
# note: if this is set to -gt 0 the /etc/hosts part is not recognized ( may be a bug )
if [[ $# -le 1 ]]; then
    usage
    exit
fi

while [[ $# -gt 0 ]]
do
    key="$1"

    case $key in
        -c|--count)
            count="$2"
            shift # past argument
            ;;
        -s|--start)
            ns_start="$2"
            shift # past argument
            ;;
        -b|--bgpsec)
            bgpsec=1
            ;;
        -d|--dry-run)
            dry_run=1
            #shift # past argument
            ;;
        -x|--prefix)
            numPrefix="$2"
            shift # past argument
            ;;
        -n|--ns-only)
            ns_only=1
            ;;
        -f|--conf-only)
            conf_only=1
            ;;
        -p|--path)
            path="$2"
            shift # past argument
            ;;
        -r|--remove)
            remove="$2"
            removeSetting
            shift # past argument
            ;;
        -h|--help)
            usage
            exit
            ;;
        *)
            # unknown option
            usage
            exit
            ;;
    esac
    shift # past argument or value
done


: ${count:=0}
: ${ns_start:=0}
: ${dry_run:=0}
: ${bgpsec:=0}
: ${numPrefix:=0}

if [[ $conf_only -ge 1 ]]; then
  conf_only="true"
fi
: ${conf_only:="false"}

if [[ $ns_only -ge 1 ]]; then
  ns_only="true"
fi
: ${ns_only:="false"}
: ${path:=""}


# limit max number
if [ $count -gt $max_count ]; then
  count=$max_count
  echo max count is limited to $max_count, please try again...
  echo
  exit
fi


info "init count: $count  start point: $ns_start  bgpsec: $bgpsec  dry-run: $dry_run \n \
     conf-only: $conf_only  ns-only: $ns_only  num-Prefix: $numPrefix\n \
     path: $path"



# prerequisite tasks
#===================
if [ "$conf_only" != "true" ]; then
    echo "+++ prerequisite tasks"
    [ "`brctl show | grep $bridge | awk '{print $1}'`" = "$bridge" ] || configure_bridge
fi


# create bgpd configuration base file (recv side)
file_recv=bgpd.conf.base
[ $bgpsec -ge 1 ] && file_recv=${file_recv}.bgpsec || file_recv=bgpd.conf.base


if [ "$ns_only" != "true" ]; then
    execute touch $file_recv

    if [[ $dry_run -eq 0 ]]; then
        baseHeader $bgpsec
    fi
fi

for ((i=$ns_start; i<$((count+ns_start)); i++)); do

  if [ "$conf_only" != "true" ]; then
    # create network namespace
    echo
    echo "+++ create network namespace nodes"
    execute ip netns add ns$i
    execute ip link add veth$((i*2)) type veth peer name veth$((i*2+1))
    execute ip link set veth$((i*2+1)) netns ns$i
    execute ip netns exec ns$i ifconfig lo up
    execute ip netns exec ns$i ifconfig veth$((i*2+1)) 10.1.1.$((start_ipaddr+i))/24 up
    execute brctl addif $bridge veth$((i*2))
    execute ip link set dev veth$((i*2)) up
    echo
  fi


  # create bgpd configuration file
  file_name=bgpd.conf.n$((i+2))
  if [[ $bgpsec -ge 1 ]]; then
    file_name=${file_name}.bgpsec
  fi


  if [ "$ns_only" != "true" ]; then
    execute touch $file_name
    echo "+++ generating bgpd file: $dir/$file_name"

    # dry run check
    if [[ $dry_run -ge 1 ]]; then
      orig_file_name=$file_name
      file_name="/dev/null"
      orig_file_recv=$file_recv
      file_recv="/dev/null"
    fi

    # file initialization
     nodeHeader $i $file_name
     generatePrefix $numPrefix "10.$((i+2))" $file_name $dry_run

     # add to server side
     echo "neighbor 10.1.1.$((i+2)) remote-as $((i+2+60000))" >> $file_recv

     nodeBodyTail $file_name
    # only bgpsec flag enabled
    if [[ $bgpsec -ge 1 ]]; then
       # add to server side
       echo neighbor 10.1.1.$((i+2)) bgpsec both >> $file_recv
    fi

    # dry run check
    if [[ $dry_run -ge 1 ]]; then
      file_name=$orig_file_name
      file_recv=$orig_file_recv
    fi
  fi


  if [ "$conf_only" != "true" ]; then
    # run bgpd (daemon mode: '-d') inside each namespace shell
    echo "+++ run bgp routing"
    execute ip netns exec ns$i bash -c "ifconfig;pwd"
    execute ip netns exec ns$i bash -c "${path}bgpd -f $dir/$file_name -i /tmp/node$((i+2))-$$.pid -d"
    echo -ne "bgpd pid: "
    execute ps aux | grep bgp[d] | grep n$((i+2)) | awk '{print $2}'
    echo "-----------------------------------------------"
  fi

echo
done


if [ "$ns_only" != "true" ]; then
    if [[ $dry_run -eq 0 ]]; then
        baseTail
    fi
fi


exit
