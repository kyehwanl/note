#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="vm058"

tmux -2 new-session -d -n "docker_test" -s "$SESSION"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]

# setup window
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/test1" C-m
tmux send-keys "docker rm `docker ps -a -q`" C-m
tmux send-keys "docker-compose -f docker-compose_github.yml up"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 1
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net test1_bgp_net --ip 172.37.0.2 --name bgpsecio           -p 1792:179 -v /opt/project/docker_test/test1/keys:/var/lib/bgpsec-keys           -v /opt/project/docker_test/test1/srxcryptoapi.conf:/etc/srxcryptoapi.conf           -v /opt/project/docker_test/test1/bgpsecio.docker.conf:/etc/bgpsecio.conf           srxsuite_src_github bgpsecio -f /etc/bgpsecio.conf" 


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 2
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net test1_bgp_net --ip 172.37.0.101 --name rpkirtr_server         -p 323:323         -v /opt/project/docker_test/test1/rpkirtr_svr.conf:/etc/rpkirtr_svr.conf         srxsuite_src_github bash"



tmux select-pane -t 3
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/test1" C-m
tmux send-keys "cat docker-compose_github.yml" C-m
tmux send-keys "telnet 172.37.0.4 2605" C-m
tmux send-keys "docker exec -it gobgpsrx bash"




# setup a new window
tmux new-window -t $SESSION:1 -n 'vim-session'  # [-t target-window] [-n window-name]
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0   # -t [arget-pane]
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/Confederation_test" C-m
tmux send-keys "docker network create --subnet=172.20.0.0/16 bgp_net" C-m
tmux send-keys "docker run --rm -it --net bgp_net --ip 172.20.0.2     -v /opt/project/docker_test/Confederation_test/bgpd_R2.conf:/etc/quagga/quagga.conf     -p 26052:2605 --name quagga2 quagga"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 1
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/Confederation_test" C-m
tmux send-keys "docker run --rm -it --net bgp_net --ip 172.20.0.10     -v /opt/project/docker_test/Confederation_test/bgpd_R1.conf:/etc/quagga/quagga.conf     -p 26051:2605 --name quagga1 quagga"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 2
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net bgp_net --ip 172.20.0.4     -v /opt/project/docker_test/Confederation_test/bgpd_R4.conf:/etc/quagga/quagga.conf     -p 26054:2605 --name quagga4 quagga"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 3
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --cap-add=NET_RAW --cap-add=NET_ADMIN --net bgp_net --ip 172.20.0.3     -v /opt/project/docker_test/Confederation_test/bgpd_R3.conf:/etc/quagga/quagga.conf     -p 26053:2605 --name quagga3 quagga bash"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 4
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net bgp_net --ip 172.20.0.5 --cap-add=NET_RAW --cap-add=NET_ADMIN     -v /opt/project/docker_test/Confederation_test/bgpd_R5.conf:/etc/quagga/quagga.conf     -p 26055:2605 --name quagga5 quagga bash"



tmux select-pane -t 5
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/Confederation_test" C-m

tmux send-keys "docker exec -it quagga5 bash" C-m
sleep 1
tmux send-keys "apt update" C-m
tmux send-keys "apt install tshark" C-m
tmux send-keys 'tshark -i eth0 -nn -f"tcp port 179" -x -V -Y"bgp.type==2"'



tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 6
tmux send-keys "ssh vmware-059" C-m
sleep 1
tmux send-keys "cd" C-m
#tmux send-keys "cd /opt/project/docker_test/Confederation_test" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/docker_test/Confederation_test --select /opt/project" C-m



# Set default window
tmux select-window -t $SESSION:0    # [-t target-window]

# Attach to session
tmux -2 attach-session -t $SESSION  # [-t target-session]







