#!/bin/sh -x
echo "srx gprc (version 6) sync with github..."
cd /tmp
git clone https://github.com/kyehwanl/srx_grpc_v6.git srx_grpc_v6

rsync -avPquiz --delete /opt/project/gobgp_test/gowork/src/srx_grpc_v6/ /tmp/_srx_grpc_v6/
cd /tmp/_srx_grpc_v6
STR=$(git log --graph --oneline --decorate --branches | head -n 1 | grep HEAD | awk -F ")" '{print $2}')
git add --all ./
git commit -m "temp-updated $(date +"%D %T")"

cd /tmp
git clone /tmp/_srx_grpc_v6 /tmp/srx_grpc0_v6
rsync -avPuiz --delete srx_grpc0_v6/ srx_grpc_v6/ --exclude='.git'

cd /tmp/srx_grpc_v6
git add --all ./
git commit -m "auto-updated $(date +"%D %T") $STR"

cp ~/Emulab/Shell_Script/input-expect.sh /tmp/
/tmp/input-expect.sh

rm -rf /tmp/srx_grpc0_v6
rm -rf /tmp/srx_grpc_v6
rm -rf /tmp/_srx_grpc_v6
rm -rf /tmp/input-expect.sh
echo "Done"
