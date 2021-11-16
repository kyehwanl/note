#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="vm005"
TMUX=/users/kyehwanl/Download/tmux-2.0/tmux

$TMUX -2 new-session -d -n "docker_test" -s "$SESSION"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]

$TMUX split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
$TMUX select-pane -t 0
$TMUX send-keys "sudo -s" C-m
$TMUX send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.2 --name exabgpsec02     -p 1792:179 -v /opt/project/docker_test/bgpsecImples/:/etc/bgpsecImples/     -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys     exabgpsec_src  bash" C-m
#$TMUX send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.2 --name bgpsecio -p 1792:179 -v /opt/project/docker_test/bgpsecImples/bgpsecio.docker.conf:/etc/bgpsecio.conf     -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys  -v /opt/project/docker_test/bgpsecImples:/etc/bgpsecImples   bgpsecio_src  bash" C-m
sleep 1
$TMUX send-keys "env exabgp.daemon.user=root exabgp /etc/exabgp.conf" C-m


$TMUX split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
$TMUX select-pane -t 1
$TMUX send-keys "sudo -s" C-m
$TMUX send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.3 --name exabgpsec03     -p 1793:179 -v /opt/project/docker_test/bgpsecImples/:/etc/bgpsecImples/      -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys     exabgpsec_src  bash" C-m
sleep 1
$TMUX send-keys "env exabgp.daemon.user=root exabgp /etc/exabgp.conf" C-m


$TMUX split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
$TMUX select-pane -t 2
$TMUX send-keys "sudo /opt/project/Github_usnistgov/NIST-GitLab/srx-server/src/rpkirtr_svr -f /opt/project/docker_test/bgpsecImples/ietf-112-hackathon/bug/701-200-data-aspa.cache" C-m


$TMUX split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
$TMUX select-pane -t 3
$TMUX send-keys "telnet 172.37.0.1 2605" C-m


$TMUX split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
$TMUX select-pane -t 4
$TMUX send-keys "sudo /opt/project/gobgp_test/gowork/src/srx_grpc_v6/quagga-srx/bgpd/.libs/bgpd -f /opt/project/docker_test/bgpsecImples/ietf-112-hackathon/bug/bgpd.conf" C-m


$TMUX select-pane -t 5
$TMUX send-keys "/opt/project/gobgp_test/gowork/src/srx_grpc_v6/srx-server/src/srx_server" C-m



# setup a new window
$TMUX new-window -t $SESSION:1 -n 'vim-session'  # [-t target-window] [-n window-name]
$TMUX select-pane -t 0   # -t [arget-pane]
$TMUX send-keys "export TERM=screen-256color" C-m
$TMUX send-keys "vim -S ~/workspace/vim-sessions/srx_grpc.vim" C-m


# setup a new window
$TMUX new-window -t $SESSION:2 
$TMUX split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
$TMUX select-pane -t 0
$TMUX send-keys "cd" C-m
$TMUX send-keys "export TERM=screen-256color" C-m
$TMUX send-keys "./vf --select /opt/project/gobgp_test/gowork/src/srx_grpc_v6 --select /opt/project/docker_test/bgpsecImples" C-m
# vifm --select <path> : open parent directory of the given path and select specified file in it

$TMUX select-pane -t 1
$TMUX send-keys "cd" C-m
$TMUX send-keys "export TERM=screen-256color" C-m
$TMUX send-keys "./vf --select /opt/project/Github_usnistgov/NIST-BGP-SRx --select /opt/project/" C-m



# Set default window
$TMUX select-window -t $SESSION:2    # [-t target-window]

# Attach to session
$TMUX -2 attach-session -t $SESSION  # [-t target-session]







