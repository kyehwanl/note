#!/bin/sh -x

echo "ExaBGPSec sync with github..."
rsync -avPuiz /opt/project/exabgp_test/exabgp-4.1.2 /tmp/
cd /tmp/exabgp-4.1.2
git remote add github_repo https://github.com/kyehwanl/exabgpsec_priv.git 
~/Emulab/Shell_Script/input-expect-github_repo.sh
rm -rf /tmp/exabgp-4.1.2
echo "Done"



