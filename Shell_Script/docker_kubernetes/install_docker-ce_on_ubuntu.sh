#! /bin/bash -x

# system update
#sudo apt update && sudo apt upgrade -y
sudo apt update -y
sudo apt-get -y install curl apt-transport-https ca-certificates software-properties-common


# Add the Docker Repositories
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
sudo apt update -y


# Install Docker on Ubuntu 18.04
sudo apt install -y docker-ce

# optional 
#sudo apt install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin
# docker-compose-plugin : docker compose [command] <-- this is not the same with 'docker-compose'



