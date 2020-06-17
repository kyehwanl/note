#!/bin/sh -x

echo "ExaBGPSec sync with github..."
rsync -avPuiz /opt/project/exabgp_test/exabgp-4.1.2 /tmp/
cd /tmp/exabgp-4.1.2
git remote add github_repo https://github.com/kyehwanlee/exabgpsec_temp.git
~/Emulab/Shell_Script/input-expect-kyehwanlee_repo.sh
rm -rf /tmp/exabgp-4.1.2
echo "Done"



