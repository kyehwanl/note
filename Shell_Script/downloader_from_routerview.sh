#!/bin/sh

#for example, input value must be : 2012.02.01
# ftp://archive.routeviews.org/bgpdata/2012.02/UPDATES/updates.20120201.1430.bz2

var=$1
: ${var:=nothing}
yyyymm=${var%.*}
yyyy=${var%%.*}
mm=${yyyymm#*.}
dd=${var##*.}

for i in $(./makeOneDay.sh); do
    echo "wget ftp://archive.routeviews.org/bgpdata/$yyyymm/UPDATES/updates.$yyyy$mm$dd.$i.bz2"
    wget ftp://archive.routeviews.org/bgpdata/$yyyymm/UPDATES/updates.$yyyy$mm$dd.$i.bz2
done

#echo $yyyymm
#echo $yyyy
#echo $mm
#echo $dd
