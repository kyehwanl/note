#!/bin/sh

name=`hostname | sed -e 's/\..*//'`
echo "run bgpd at $name..."
sudo $PWD/020-03/quagga-0.99.16-srx-0.2/bgpd/bgpd -f $name/bgpd.$name.conf -d
