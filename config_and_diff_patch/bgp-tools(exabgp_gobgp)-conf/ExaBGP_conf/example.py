#!/usr/bin/env python

from sys import stdout
from time import sleep

messages = [
#'announce route 30.30.0.0/24 next-hop self',
'announce route 30.30.0.0/24 next-hop 10.1.1.2',
#'announce route 30.40.0.0/24 next-hop self',
'announce route 30.40.0.0/24 next-hop 10.1.1.3',
#'announce route 30.50.0.0/24 next-hop self',
#'announce route 30.60.0.0/24 next-hop self',
#'announce route 30.70.0.0/24 next-hop self',
#'announce route 30.80.0.0/24 next-hop self',
#'announce route 30.90.0.0/24 next-hop self'
]

sleep(5)

#Iterate through messages
for message in messages:
    stdout.write( message + '\n')
    stdout.flush()
    sleep(1)

#Loop endlessly to allow ExaBGP to continue running
while True:
    sleep(1)

'''
Important note
==============

Implementators using this API should keep in mind that read on STDIN are normally blocking. When using the same process for SENDING and receiving, ASYNC IO programming techniques should be used.

However two processes, one for sending and one for receiving, can be used, which may make the programming task easier if shared states are stored in a common database / key store / NoSQL application.
'''
