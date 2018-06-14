#!/bin/sh -x

echo "ExaBGP sync with github..."
cp -rf ~/Emulab/exabgp-3.4.13/ /tmp/
cd /tmp/exabgp-3.4.13
git remote add origin https://github.com/kyehwanl/exabgp_priv
~/Emulab/Shell_Script/input-expect.sh
rm -rf /tmp/exabgp-3.4.13
echo "Done"



