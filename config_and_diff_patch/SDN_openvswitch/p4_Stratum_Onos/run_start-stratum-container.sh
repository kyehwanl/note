#!/bin/bash
set -x
docker run -it --rm --privileged \
        -v /dev:/dev -v /sys:/sys -v /lib/modules/4.14.49-OpenNetworkLinux:/lib/modules/4.14.49-OpenNetworkLinux \
        --env PLATFORM=x86-64-accton-wedge100bf-32x-r0 \
        -p 28000:28000 -p 9339:9339 -p 9559:9559 \
        -v /lib/x86_64-linux-gnu/libonlp-platform-defaults.so:/lib/x86_64-linux-gnu/libonlp-platform-defaults.so \
        -v /lib/x86_64-linux-gnu/libonlp-platform-defaults.so.1:/lib/x86_64-linux-gnu/libonlp-platform-defaults.so.1 \
        -v /lib/x86_64-linux-gnu/libonlp-platform.so:/lib/x86_64-linux-gnu/libonlp-platform.so \
        -v /lib/x86_64-linux-gnu/libonlp-platform.so.1:/lib/x86_64-linux-gnu/libonlp-platform.so.1 \
        -v /lib/x86_64-linux-gnu/libonlp.so:/lib/x86_64-linux-gnu/libonlp.so \
        -v /lib/x86_64-linux-gnu/libonlp.so.1:/lib/x86_64-linux-gnu/libonlp.so.1 \
	-v /lib/x86_64-linux-gnu/libonlp-x86-64-accton-wedge100bf-32x.so.1:/lib/x86_64-linux-gnu/libonlp-x86-64-accton-wedge100bf-32x.so.1 \
	-v /root/chassis_config.pb.txt:/etc/stratum/x86-64-accton-wedge100bf-32x-r0/chassis_config.pb.txt \
	-v /var/log:/var/log/stratum \
	stratumproject/stratum-bf:9.2.0 \
	$@
