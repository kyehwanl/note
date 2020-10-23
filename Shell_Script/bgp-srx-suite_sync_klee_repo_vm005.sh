#!/bin/sh -x

echo "NIST-BGP-SRx sync with github..."
rsync -avPuiz /opt/project/Github_usnistgov/NIST-BGP-SRx /tmp/
cd /tmp/NIST-BGP-SRx
git remote add github_repo https://github.com/kyehwanlee/bgp-srx-suite.git

STR=$(git log --graph --oneline --decorate --branches | head -n 1 | grep HEAD | awk -F ")" '{print $2}')
git add --all ./
git commit -m "auto-updated $(date +"%D %T") $STR"

~/Emulab/Shell_Script/input-expect-kyehwanlee_repo.sh
rm -rf /tmp/NIST-BGP-SRx
echo "Done"



