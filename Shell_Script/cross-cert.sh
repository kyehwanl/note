#!/bin/bash

function execute()
{
 echo -ne " -> Press enter to execute command:\n\033[1m$@\033[0m"
 read
 echo -ne "\E[32m"
 sh -c "$@"
 echo -e "\033[0m -> done"
 echo
}

function info()
{
 echo
 echo "##########################################################"
 echo -e " \033[1m$@\033[0m"
 echo "##########################################################"
 echo
}

function ptc()
{
 echo "Press enter key to continue..."
 read
 echo
}


echo +++++++++++++++++++++++++++++
echo + CA Cross Signing DEMO     +
echo +++++++++++++++++++++++++++++ 
echo 
echo Trusts before cross-signing:
echo
echo "On Entity A: A_ca -> A_child"
echo "On Entity B: B_ca -> B_child"
echo
echo Additinal trusts after cross-signing:
echo
echo "On Entity A: A_ca -> B_ca -> B_child "
echo "On Entity B: B_ca -> A_ca -> A_child "
echo

ptc

mkdir -p /tmp/cross-cert
cd /tmp/cross-cert
mkdir -p entity_A entity_B exchange

cd entity_A
info Entity A: Generate private keys
execute "openssl genrsa -out A_ca.key 2048"
execute "openssl genrsa -out A_child.key 2048"

info Entity A: create CA cert
execute "openssl req -new -x509 -key A_ca.key -out A_ca.crt -batch -subj '/CN=A CA/'"
info Entity A: child certificate signing request
execute "openssl req -new -key A_child.key -out A_child.req -batch -subj '/CN=A child'"
info Entity A: child signing by CA
execute "openssl x509 -req -CA A_ca.crt -in A_child.req -CAkey A_ca.key -set_serial 0 -out A_child.crt"
info Entity A: verify child cert
execute "openssl verify -verbose -CAfile A_ca.crt A_child.crt"

info Entity A is setup, now doing the same for entity B



cd ../entity_B
execute "openssl genrsa -out B_ca.key 2048"
execute "openssl genrsa -out B_child.key 2048"
execute "openssl req -new -x509 -key B_ca.key -out B_ca.crt -batch -subj '/CN=B CA/'"
execute "openssl req -new -key B_child.key -out B_child.req -batch -subj '/CN=B child/'"
execute "openssl x509 -req -CA B_ca.crt -in B_child.req -CAkey B_ca.key -set_serial 0 -out B_child.crt"
execute "openssl verify -verbose -CAfile B_ca.crt B_child.crt"
info Entity B finished
cd ..
info Entity A: convert CA cert to certificate signing request for CA cross signing 
execute "openssl x509 -x509toreq -in entity_A/A_ca.crt -signkey entity_A/A_ca.key -out exchange/A_ca.req"
info Same on entity B
execute "openssl x509 -x509toreq -in entity_B/B_ca.crt -signkey entity_B/B_ca.key -out exchange/B_ca.req"


info Entity B: sign A CA cert
execute "openssl x509 -req -in exchange/A_ca.req -CA entity_B/B_ca.crt -CAkey entity_B/B_ca.key -set_serial 1 -out entity_B/A_ca_signed_by_B_ca.crt"
info Entity A: sign B CA cert
execute "openssl x509 -req -in exchange/B_ca.req -CA entity_A/A_ca.crt -CAkey entity_A/A_ca.key -set_serial 1 -out entity_A/B_ca_signed_by_A_ca.crt"


info Verify cross signing
execute "openssl verify -CAfile entity_A/A_ca.crt entity_A/B_ca_signed_by_A_ca.crt"
execute "openssl verify -CAfile entity_B/B_ca.crt entity_B/A_ca_signed_by_B_ca.crt"



info Fill validation directories
execute "c_rehash entity_A"
execute "c_rehash entity_B"



info Check all child certs vs both trust chains
execute "openssl verify -CApath entity_A/ entity_A/A_child.crt" 
execute "openssl verify -CApath entity_A/ entity_B/B_child.crt" 
execute "openssl verify -CApath entity_B/ entity_B/B_child.crt" 
execute "openssl verify -CApath entity_B/ entity_A/A_child.crt"

info "Certificates written to /tmp/cross-cert"

