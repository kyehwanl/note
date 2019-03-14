#!/bin/sh -x
echo "srx gprc sync with github..."
cd /tmp
git clone https://github.com/kyehwanl/srx_grpc.git srx_grpc

cp -rf /opt/project/gobgp_test/gowork/src/srx_grpc /tmp/srx_grpc0
rsync -avPuiz --delete srx_grpc0/ srx_grpc/ --exclude='.git'

cd srx_grpc
git add --all ./
git commit -m "auto-updated $(date +"%D %T")"

cp ~/Emulab/Shell_Script/input-expect.sh /tmp/
/tmp/input-expect.sh

rm -rf /tmp/srx_grpc0
rm -rf /tmp/srx_grpc
rm -rf /tmp/input-expect.sh
echo "Done"

