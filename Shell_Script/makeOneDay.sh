#!/bin/sh

function suffix()
{
    for n in 0 1; do
	for((i=0; i<10; i++)); do 
	    for j in 0 1 3 4; do
		if [ "$j" = "0" ] || [ "$j" = "3" ] ; then
		    echo $n$i$j'0' 
		else
		    echo $n$i$j'5' 
		fi
	    done
	done
    done

    for((i=0; i<4; i++)); do 
	for j in 0 1 3 4; do
	    if [ "$j" = "0" ] || [ "$j" = "3" ] ; then
		echo 2$i$j'0' 
	    else
		echo 2$i$j'5' 
	    fi
	done
    done
}

suffix

