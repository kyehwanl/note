#!/bin/sh -x

echo "Quaggasrx synchronizing with github..."
cp -rf /users/kyehwanl/Quagga_test/Proces_Performance/QuaggaSRxSuite/qsrx /tmp/
cd /tmp/qsrx
git remote add origin https://github.com/kyehwanl/qsrx_priv
/users/kyehwanl/Shell_Script/input-expect.sh
rm -rf /tmp/qsrx
echo "done"

echo "SRx Server synchronizing with github..."
cp -rf /users/kyehwanl/Quagga_test/Proces_Performance/QuaggaSRxSuite/srx /tmp/
cd /tmp/srx
git remote add origin https://github.com/kyehwanl/srx_priv
/users/kyehwanl/Shell_Script/input-expect.sh
rm -rf /tmp/srx
echo "done"


echo "API synchronizing with github..."
cp -rf /users/kyehwanl/Quagga_test/Proces_Performance/QuaggaSRxSuite/api /tmp/
cd /tmp/api
git remote add origin https://github.com/kyehwanl/api_priv
/users/kyehwanl/Shell_Script/input-expect.sh
rm -rf /tmp/api
echo "done"
