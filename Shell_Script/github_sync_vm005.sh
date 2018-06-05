#!/bin/sh -x

cd /tmp
sudo git clone https://github.com/kyehwanl/note
sudo chown -R kyehwanl note

rsync -avPuiz --delete ~/Emulab/config_and_diff_patch/ note/config_and_diff_patch/
rsync -avPuiz --delete ~/Emulab/Shell_Script/ note/Shell_Script/
rsync -avPuiz --delete ~/Emulab/DEBUGGING_NOTE/ note/DEBUGGING_NOTE/

cd note
git add .
git commit -m 'auto-updated'

cp ~/Emulab/Shell_Script/input-expect.sh /tmp/
sudo /tmp/input-expect.sh

rm -rf /tmp/note
rm -rf /tmp/input-expect.sh



