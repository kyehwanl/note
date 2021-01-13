#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="gobgp"

tmux -2 new-session -d -s "$SESSION" -n "vim_window"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session
# [-s session-name] [-n window-name]


# Set up layout for all windows
# Setup 1st window 
# Setup gobgp pane
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0   # -t [arget-pane]
tmux send-keys "cd /home/kyehwanl/project/gobgp_v2_running_test/src/github.com/osrg/gobgp" C-m
tmux send-keys "sudo $GOPATH/bin/dlv exec /home/kyehwanl/project/gobgp_v2_running_test/bin/gobgpd -- -f /home/kyehwanl/project/gobgp_v2_running_test/bin/gobgpd.ns1.conf.bgpsec -p" C-m
tmux send-keys "c" C-m

# Setup quagga bgpd pane
tmux select-pane -t 1
tmux send-keys "sudo -s" C-m
tmux send-keys "ip netns del ns1" C-m
tmux send-keys "./netns_peer_gen" C-m
sleep 1
tmux send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1>\"")' C-m
tmux send-keys "/home/kyehwanl/project/QuaggaSRxSuite/_inst/sbin/bgpd -f /home/kyehwanl/project/bgpd.n2.conf.bgpsec" C-m

# Setup a gobgp client pane
tmux split-window -v
tmux send-keys "cd /home/kyehwanl/project/gobgp_v2_running_test/bin" C-m
tmux send-keys "dlv exec ./gobgp -- global rib" C-m
tmux resize-pane -U 50  # towwards Upside increase 50 lines
tmux resize-pane -U 20


# Setup 2nd window
tmux new-window -t $SESSION:1 -n 'gobgp_v1'  # [-t target-window] [-n window-name]
tmux select-pane -t 0
tmux send-keys "cd /home/kyehwanl/project/gowork/src/github.com/osrg/gobgp" C-m

# Setup 3rd window
tmux new-window -t $SESSION:2 -n 'gobgp_v2' 
tmux select-pane -t 0
tmux send-keys "cd /home/kyehwanl/project/gobgp_v2_running_test/src/github.com/osrg/gobgp" C-m
tmux send-keys "vim" C-m


# Set default window
tmux select-window -t $SESSION:2    # [-t target-window]

# Attach to session
tmux -2 attach-session -t $SESSION  # [-t target-session]

