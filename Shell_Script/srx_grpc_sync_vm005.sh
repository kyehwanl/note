#!/bin/sh -x
echo "srx gprc sync with github..."
cd /tmp
git clone https://github.com/kyehwanl/srx_grpc.git srx_grpc

rsync -avPquiz --delete /opt/project/gobgp_test/gowork/src/srx_grpc/ /tmp/_srx_grpc/
cd /tmp/_srx_grpc
STR=$(git log --graph --oneline --decorate --branches | head -n 1 | grep HEAD | awk -F ")" '{print $2}')
git add --all ./
git commit -m "temp-updated $(date +"%D %T")"

cd /tmp
git clone /tmp/_srx_grpc /tmp/srx_grpc0
rsync -avPuiz --delete srx_grpc0/ srx_grpc/ --exclude='.git'

cd /tmp/srx_grpc
git add --all ./
git commit -m "auto-updated $(date +"%D %T") $STR"

cp ~/Emulab/Shell_Script/input-expect.sh /tmp/
/tmp/input-expect.sh

rm -rf /tmp/srx_grpc0
rm -rf /tmp/srx_grpc
rm -rf /tmp/_srx_grpc
rm -rf /tmp/input-expect.sh
echo "Done"

