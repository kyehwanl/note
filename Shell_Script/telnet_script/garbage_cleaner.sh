#!/bin/sh
for i in $(seq 1 6); do
  ls  node$i/bgpd.log  && rm -rf node$i/bgpd.log
  ls  node0$i/bgpd.log && rm -rf node0$i/bgpd.log
  rm -rf node$i/*~ | rm -rf node0$i/*~
done

