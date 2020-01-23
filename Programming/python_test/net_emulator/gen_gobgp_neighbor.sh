#!/bin/bash 



for ((i=2; i<102; i++)); do  
    echo
    echo '[[neighbors]]'
    echo '  [neighbors.config]'
    echo '    neighbor-address = "10.1.1.'$i'"'
    peer=$((60000+i))
    echo '    peer-as = '$peer
    echo '    bgpsec-enable = true'
    echo '    SKI = "C30433FA1975FF193181458FB902B501EA9789DC"' 
    echo

done










#[[neighbors]]
#  [neighbors.config]
#    neighbor-address = "10.1.1.3"
#    peer-as = 60003
#    bgpsec-enable = true
#    SKI = "C30433FA1975FF193181458FB902B501EA9789DC"



