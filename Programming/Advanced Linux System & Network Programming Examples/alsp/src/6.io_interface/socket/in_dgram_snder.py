#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# UDP sender
import socket
import sys
import string
import time

# ---------------------------[ End of Variable definition ]-------------------------------
# DESC : 사용법을 출력한다.
# ARGV : none
# RET  : none
def usages():
    print "Usage :", sys.argv[0], "<host> <port>"
    sys.exit()
# ---------------------------[ End of Function Definition ]-------------------------------

if len(sys.argv) != 3:
    usages()

HOST = sys.argv[1]
PORT = int(sys.argv[2])
print "Will sendto to ", HOST, PORT
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sbuffer = '[Client] Echo client Testing program'
while 1:
    s.sendto(sbuffer, 0, (HOST, PORT))
    print "[Send:" + str(len(sbuffer)) + "] ", sbuffer
    rbuffer = s.recvfrom(2048)
    print "[Recv:" + str(len(rbuffer[0])) + "] addr =", rbuffer[1] # rbuffer[0] is data, [1] is address
    time.sleep(1)
s.close()
