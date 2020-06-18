#!/bin/sh -x

echo "GoBGPSec sync with github..."
rsync -avPuiz /opt/project/gobgp_test/gobgp_v2_running_test/src/github.com/osrg/gobgp /tmp/
cd /tmp/gobgp
git remote add github_repo https://github.com/kyehwanlee/gobgpsec_temp.git

STR=$(git log --graph --oneline --decorate --branches | head -n 1 | grep HEAD | awk -F ")" '{print $2}')
git add --all ./
git commit -m "auto-updated $(date +"%D %T") $STR"

~/Emulab/Shell_Script/input-expect-kyehwanlee_repo.sh
rm -rf /tmp/gobgp
echo "Done"



