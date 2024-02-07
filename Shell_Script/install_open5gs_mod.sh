#! /bin/bash
# Installing Open5Gs - 5G in a Box
# https://open5gs.org/open5gs/docs/guide/01-quickstart/

# Getting MongoDB
sudo apt update
sudo apt install gnupg -y
curl -fsSL https://pgp.mongodb.com/server-6.0.asc | sudo gpg -o /usr/share/keyrings/mongodb-server-6.0.gpg --dearmor
echo "deb [ arch=amd64,arm64 signed-by=/usr/share/keyrings/mongodb-server-6.0.gpg] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/6.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-6.0.list

# Install MongoDB
sudo apt update
sudo apt install -y mongodb-org
sudo systemctl start mongod 
sudo systemctl enable mongod 

# Install Open5gs 
sudo add-apt-repository ppa:open5gs/latest
sudo apt update
sudo apt install open5gs -y


#---------------------------------
# Either one of them (1 or 2) 
# 	 needs to be chosen
#---------------------------------

#---------------------------------
# 1. mohitkr05 
# 	https://github.com/mohitkr05/5G_stuff/blob/main/vagrant/04_multiplevms/installation/install_open5gs.sh

# Install NodeJS and WebUI
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt install -y nodejs
sudo mkdir -p /usr/lib/node_modules/open5gs
sudo curl -fsSL https://open5gs.org/open5gs/assets/webui/install | sudo -E bash -
sudo cp /home/vagrant/data/open5gs_webui.service /lib/systemd/system/open5gs-webui.service
sudo systemctl daemon-reload
sudo systemctl restart open5gs-webui



#---------------------------------
# 2. Official web site
# 	https://open5gs.org/open5gs/docs/guide/01-quickstart/#:~:text=restart%20open5gs%2Dsgwud-,Setup%20a%205G%20Core,-You%20will%20need

# Download and import the Nodesource GPG key
sudo apt update
sudo apt install -y ca-certificates curl gnupg
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg

# Create deb repository
export NODE_MAJOR=20
echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_MAJOR.x nodistro main" | sudo tee /etc/apt/sources.list.d/nodesource.list

# Run Update and Install
sudo apt update
sudo apt install nodejs -y

# Install WebUI of Open5GS.
curl -fsSL https://open5gs.org/open5gs/assets/webui/install | sudo -E bash -








#apt-get -y update
#apt-get install -y gnupg wget tcpdump 
#wget -qO - https://www.mongodb.org/static/pgp/server-6.0.asc > key.asc 
#sudo apt-key add key.asc
#echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/6.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-6.0.list
#apt-get -y update
#apt install -y mongodb-org mongodb-clients
#sudo systemctl start mongod
#sudo systemctl enable mongod
#add-apt-repository ppa:open5gs/latest
#apt-get install -y software-properties-common
#apt-get -y update
#apt install -y open5gs
##sudo apt install libc6=2.31-0ubuntu9.2 libc-bin=2.31-0ubuntu9.2 - To be used if getting error for nodejs
#curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
#sudo apt install -y nodejs
#mkdir -p /usr/lib/node_modules/open5gs
#sudo curl -fsSL https://open5gs.org/open5gs/assets/webui/install | sudo -E bash -
#sudo cp /home/vagrant/data/open5gs_webui.service /lib/systemd/system/open5gs-webui.service
#sudo systemctl daemon-reload
#sudo systemctl restart open5gs-webui
