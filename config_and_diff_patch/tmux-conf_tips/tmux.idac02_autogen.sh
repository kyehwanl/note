#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="idac02"

tmux -2 new-session -d -n "docker_test" -s "$SESSION"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]

# setup window
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/bgpsecImples/" C-m
tmux send-keys "docker-compose rm -fs" C-m
tmux send-keys "docker-compose up" C-m


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 1
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.3 --name gobgpsec -p 1793:179 -v /opt/projt/docker_test/bgpsecImples/gobgpd.docker.conf:/etc/gobgpd.conf -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys gobgpsec_src gobgpd -p -f /etc/gobgpd.conf --log-level=debug" C-m


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 2
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.32 --name exabgpsec -p 1792:179 -v /opt/project/docker_test/bgpsecImples/exabgp.docker.conf:/etc/exabgp.conf -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys exabgpsec_src" C-m



tmux select-pane -t 3
tmux send-keys "sudo -s" C-m
tmux send-keys "/opt/project/TestBgpsecImple/netns_peer_gen" C-m
sleep 1
tmux send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1> \"")' C-m
tmux send-keys "cd /opt/project/TestBgpsecImple" C-m
tmux send-keys "bgpd -f /opt/project/TestBgpsecImple/bgpd.n2.conf.bgpsec -u root -i /tmp/bgpd.pid" C-m


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 4
tmux send-keys "sudo -s" C-m
tmux send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1> \"")' C-m
tmux send-keys "cd /opt/project/TestBgpsecImple/" C-m
tmux send-keys "./srx_server" C-m



tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 5
tmux send-keys "sudo -s" C-m
tmux send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.4 --name quaggasrx -p 1791:179 -p 2605:2605 -v /opt/project/docker_test/bgpsecImples/quaggasrx.docker.conf:/etc/quagga/bgpd.conf -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys srxsuite_src bgpd -u root -f /etc/quagga/bgpd.conf" C-m




# setup a new window
tmux new-window -t $SESSION:1 -n 'vifm' 
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/gobgp_test --select /opt/project/TestBgpsecImple/test_install" C-m


tmux select-pane -t 1
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf" C-m



# Set default window
tmux select-window -t $SESSION:1    # [-t target-window]

# Attach to session
tmux -2 attach-session -t $SESSION  # [-t target-session]







