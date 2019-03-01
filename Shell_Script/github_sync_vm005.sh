#!/bin/sh -x

cd /tmp
sudo git clone https://github.com/kyehwanl/note
sudo chown -R kyehwanl note

mkdir note0
cp -rf ~/Emulab/config_and_diff_patch/ /tmp/note0
cp -rf ~/Emulab/Shell_Script/ /tmp/note0
cp -rf ~/Emulab/DEBUGGING_NOTE/  /tmp/note0
cp -rf ~/Emulab/Documents/  /tmp/note0
cp -rf ~/Emulab/Programming/  /tmp/note0

rsync -avPuiz --delete note0/config_and_diff_patch/ note/config_and_diff_patch/
rsync -avPuiz --delete note0/Shell_Script/ note/Shell_Script/
rsync -avPuiz --delete note0/DEBUGGING_NOTE/ note/DEBUGGING_NOTE/
rsync -avPuiz --delete note0/Documents/ note/Documents/
rsync -avPuiz --delete note0/Programming/ note/Programming/ --exclude='.git*'

cd note
git add --all ./
git commit -m "auto-updated $(date +"%D %T")"

cp ~/Emulab/Shell_Script/input-expect.sh /tmp/
sudo /tmp/input-expect.sh

cd /tmp
rm -rf /tmp/note /tmp/note0
rm -rf /tmp/input-expect.sh



