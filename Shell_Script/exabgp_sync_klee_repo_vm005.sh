#!/bin/sh -x

echo "ExaBGPSec sync with github..."
rsync -avPuiz /opt/project/exabgp_test/exabgp-4.1.2 /tmp/
cd /tmp/exabgp-4.1.2
git remote add github_repo https://github.com/kyehwanlee/exabgpsec_temp.git

STR=$(git log --graph --oneline --decorate --branches | head -n 1 | grep HEAD | awk -F ")" '{print $2}')
git add --all ./
git commit -m "auto-updated $(date +"%D %T") $STR"

~/Emulab/Shell_Script/input-expect-kyehwanlee_repo.sh
rm -rf /tmp/exabgp-4.1.2
echo "Done"



