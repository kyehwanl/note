#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="idac03"

tmux -2 new-session -d -n "docker_gobgp" -s "$SESSION"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]


# setup window
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "sudo -s" C-m
tmux send-keys "cd /opt/project/docker_test/bgpsecImples/" C-m
tmux send-keys "docker-compose rm -fs" C-m
tmux send-keys "docker-compose -f ./docker-compose.yml up" C-m


tmux select-pane -t 1
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/gobgp_test --select /opt/project/docker_test/bgpsecImples" C-m



tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 2
tmux send-keys "sudo docker network ls" C-m
tmux send-keys "telnet 172.37.0.4 2605" C-m



# setup a new window
tmux new-window -t $SESSION:1 -n 'vifm' 
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/docker_test/bgpsecImples --select ~/config_and_diff_patch/bgpd-tools-conf" C-m



tmux select-pane -t 1
tmux send-keys "sudo -s" C-m
tmux send-keys "docker images" C-m
tmux send-keys "docker run --rm -it --hostname golang golang" C-m


# Set default window
tmux select-window -t $SESSION:1    # [-t target-window]

# Attach to session
tmux -2 attach-session -t $SESSION  # [-t target-session]







