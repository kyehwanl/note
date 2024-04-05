#!/bin/bash

function info()                                                      
{                                                                    
 echo  
 echo "---------------------------------------------------------"    
 echo -e " \033[1m$@\033[0m" 
 echo "---------------------------------------------------------"  
 echo
}                                                                    

function run_execute()
{
  for i in $(seq $init $max); do

    if [ "$ip" = "" ]; then
      ipaddress=""
    else
      ipaddress=$ip$(($i+1)) 
    fi

    if [ "$type" != "exabgp" ]; then
      echo "docker run --name node$(($i+1)) -d -v $path/bgpd.conf.n$(($i+1)):/etc/quagga/bgpd.conf $net $ipaddress 10.0.50.11:5000/srx"
      docker run --name node$(($i+1)) -d -v $path/bgpd.conf.n$(($i+1)):/etc/quagga/bgpd.conf $net $ipaddress 10.0.50.11:5000/srx
    else
      echo "docker run --name node$(($i+1)) -d -v $path/bgpd.conf.n$(($i+1)):/etc/exabgp/exabgp.conf $net $ipaddress pierky/exabgp"
      docker run --name node$(($i+1)) -d -v $path/bgpd.conf.n$(($i+1)):/etc/exabgp/exabgp.conf $net $ipaddress pierky/exabgp
    fi
  done
}

function run_test()
{

  for i in $(seq $init $max); do

    if [ "$ip" = "" ]; then
      ipaddress=""
    else
      ipaddress=$ip$(($i+1)) 
    fi

    if [ "$type" != "exabgp" ]; then
      echo "docker run --name node$(($i+1)) -d -v $path/bgpd.conf.n$(($i+1)):/etc/quagga/bgpd.conf $net $ipaddress 10.0.50.11:5000/srx"
    else
      echo "docker run --name node$(($i+1)) -d -v $path/bgpd.conf.n$(($i+1)):/etc/exabgp/exabgp.conf $net $ipaddress pierky/exabgp"
    fi
  done
}

function stop_and_remove
{
  for i in $(seq $init $max); do
	echo "docker stop node$(($i+1))"
    docker stop node$(($i+1))
  done

  for i in $(seq $init $max); do
	echo "docker rm node$(($i+1))"
    docker rm node$(($i+1))
  done
	#for i in $(docker ps -q); do
	#	echo "docker stop $i"
	#	docker stop $i
	#done

	#for i in $(docker ps -aq); do
	#	echo "docker rm $i"
	#	docker rm $i
	#done
}



function usage() {
    echo 
    echo "usage: $0 [-o <test|run|stop>] [-m <max_num>] [-t <quagga|exabgp>] [-n <subnet> -i <A.B.C.>]" 
    echo "  options: "                       
    echo -e "\t-o, --option     : <value> test for printout command or execute real command or stop all dockers"
    echo -e "\t-m, --max        : max number of instances"                 
    echo -e "\t-s, --start      : start number of instances"                 
    echo -e "\t-t, --target     : <value> target operation mode, quagga or exabgp"
    echo -e "\t-n, --net        : specify subnet name without the last digit"
    echo -e "\t-i, --ip         : specify subnet ip '--net xxx.xxx.xxx.' with the 3rd period"
    echo -e "\t-h, --help       : help screen"                                           
    echo                                                                                 
}


# Use -gt 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use -gt 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it such                                                                                                                                                                                                            # as in the --default example).
# note: if this is set to -gt 0 the /etc/hosts part is not recognized ( may be a bug ) 


if [[ $# -le 1 ]]; then
    usage
    exit
fi 


while [[ $# -gt 0 ]]
do
    key="$1"  
    case $key in
        -o|--option)
            option="$2"
            shift # past argument
            ;;
        -s|--start)
            init="$2"
            shift # past argument
            ;;
        -m|--max)
            max="$2"
            shift # past argument
            ;;
        -t|--type)
            type="$2"
            shift # past argument
            ;;
        -n|--net)
            net="--net $2"
            shift # past argument
            ;;
        -i|--ip)
            ip="--ip $2"
            shift # past argument
            ;;
        -p|--path)
            path="$2"
            shift
            ;;
        -h|--help)
            usage
            exit
            ;;
        *)       # unknown option
            usage
            exit
            ;;
    esac
    shift # past argument or value
done


: ${option:="test"}
: ${max:=1}
: ${init:=1}
: ${type:="quagga"}
: ${net:=""}
: ${ip:=""}
: ${path:=$PWD}


max_count=500
# limit max number
if [[ $count -gt $max_count ]]; then
  count=$max_count
  echo max count is limited to $max_count, please try again...
  echo
  exit
fi


info "option: $option \n start: $init \n max: $max \n type:$type \n net: $net \n ip: $ip \n path: $path"


if [ "$option" = "test" ]; then
  	run_test 
elif [ "$option" = "run" ]; then
  	run_execute 
elif [ "$option" = "stop" ]; then
	stop_and_remove
fi


