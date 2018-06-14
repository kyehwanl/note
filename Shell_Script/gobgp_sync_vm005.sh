#!/bin/sh -x

echo "GoBGP sync with github..."
cp -rf /opt/project/gobgp_test/gowork/src/github.com/osrg/gobgp/ /tmp/
cd /tmp/gobgp
git remote add github_repo https://github.com/kyehwanl/gobgp_priv
~/Emulab/Shell_Script/input-expect-github_repo.sh
rm -rf /tmp/gobgp
echo "Done"



