#! /bin/bash -x

# optional 
#sudo apt install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin
# docker-compose-plugin : docker compose [command] <-- this is not the same with 'docker-compose'

# need to download the stable/latest from the release page, 
# https://github.com/docker/compose/releases
# https://github.com/docker/compose/releases/download/v2.26.0/docker-compose-linux-x86_64

sudo curl -L "https://github.com/docker/compose/releases/download/2.26.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
# uname -s : linux, uname -m: x86_64

sudo chmod +x /usr/local/bin/docker-compose
docker-compose --version


# output
#  $ docker-compose --version
#  Docker Compose version v2.26.0


