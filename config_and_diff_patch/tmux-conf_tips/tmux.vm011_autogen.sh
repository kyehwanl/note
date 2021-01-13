#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="vm011"

tmux -2 new-session -d -n "exaBgpsec_to_goBgpsec(vm005)" -s "$SESSION"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]



# setup window
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "cd /opt/project/exabgp_test/exabgp-4.1.2/" C-m
tmux send-keys "env exabgp.daemon.user=root ./sbin/exabgp ./exabgp.nsBase.conf" C-m


tmux select-pane -t 1
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/exabgp_test/exabgp-4.1.2 --select /opt/project/exabgp_test" C-m



# setup a new window
tmux new-window -t $SESSION:1 -n 'grpc-web' 
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/gRPC_test/gRPC-Web/grpc-web --select /opt/project/docker_test/srx/compose_test" C-m



tmux select-pane -t 1
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/gRPC_test/gRPC-Web/grpc-web --select /opt/project/srx_test1" C-m


# Set default window
tmux select-window -t $SESSION:1    # [-t target-window]

# Attach to session
tmux -2 attach-session -t $SESSION  # [-t target-session]







