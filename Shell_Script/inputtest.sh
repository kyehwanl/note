#!/bin/sh

while read inputline
do
	login="$(echo $inputline | cut -d: -f1)"
	fulln="$(echo $inputline | cut -d: -f4)"
	#echo login = $login and fullname = $fulln
done < /etc/passwd

cd $RPKI_TS2
python sql-setup.py << eof
ts2@B211
eof
read x < /users/kyehwanl/testinput
echo $x

exit 0
