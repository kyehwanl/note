#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# UNIX domain socket with SOCK_STREAM
import socket
import sys
import string
import time

# ---------------------------[ End of Variable definition ]-------------------------------
# DESC : 사용법을 출력한다.
# ARGV : none
# RET  : none
def usages():
    print "Usage :", sys.argv[0], "<path>"
    sys.exit()
# ---------------------------[ End of Function Definition ]-------------------------------
if len(sys.argv) != 2:
    usages()

PATH = sys.argv[1]
print "Will send to ", PATH
s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
s.connect((PATH))
sbuffer = '[Client] Echo client Testing program'
while 1:
    s.send(sbuffer, 0)
    print "[Send:" + str(len(sbuffer)) + "] ", sbuffer
    rbuffer = s.recv(2048)
    print "[Recv:" + str(len(rbuffer[0])) + "]"
    time.sleep(1)
s.close()
