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
