#!/bin/sh -x

echo "GoBGPSec sync with github..."
rsync -avPuiz /opt/project/gobgp_test/gobgp_v2_running_test/src/github.com/osrg/gobgp /tmp/
cd /tmp/gobgp
git remote add github_repo https://github.com/kyehwanl/gobgpsec_priv.git
~/Emulab/Shell_Script/input-expect-github_repo.sh
rm -rf /tmp/gobgp
echo "Done"



