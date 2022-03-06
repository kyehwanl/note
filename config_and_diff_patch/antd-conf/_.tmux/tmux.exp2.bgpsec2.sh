#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
SESSION=$USER

tmux -2 new-session -d -s "$SESSION"

# Setup a window for tailing log files
#tmux new-window -t $SESSION:1 -n 'Logs'
tmux split-window -h
tmux select-pane -t 0
tmux send-keys "cd /users/kyehwanl/Downloads/bgpsec-test/bird-1.3.9-bgpsec3" C-m
tmux send-keys "sudo ./bird -c bird.bgpsec.BGPSEC2.conf" C-m

tmux select-pane -t 1
tmux send-keys "ssh node1.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
sleep 1
tmux send-keys "Lee@A218" C-m
tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -i /tmp/bgpd.pid -f ~/srx_test1/node1/bgpd.bgpsec.bird.conf" C-m

# Setup a watchdog pane
tmux split-window -v
tmux resize-pane -D 20
tmux send-keys "sudo tail -f /var/log/messages | grep bird" C-m

# Setup a watchdog pane
tmux select-pane -t 0
tmux split-window -v -p 50
tmux send-keys "ssh bgpsec1.antd.nist.gov" C-m
sleep 2
tmux send-keys "Tlvus119:9,11" C-m
sleep 2
tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -f ~/srx_test1/node2/bgpd.BGPSEC.bs1.500k.conf" C-m

tmux split-window -v -p 50
tmux send-keys "cd /users/kyehwanl/Downloads/bgpsec-test/bird-1.3.9-bgpsec3" C-m
tmux send-keys "sudo ./birdc" C-m
tmux send-keys "show protocols all" C-m

tmux split-window -v -p 50
tmux send-keys "telnet node1.bs1.bgp-srx.emulab2.antd.nist.gov 2605" C-m
sleep 1
tmux send-keys "z" C-m
sleep 1
tmux send-keys "sh ip bgp summary" C-m


# Set up layout for all panes
tmux select-layout tiled


# Setup a MySQL window
#tmux new-window -t $SESSION:2 -n 'MySQL' 'mysql -uroot'

# Set default window
tmux select-window -t $SESSION:1

# Attach to session
tmux -2 attach-session -t $SESSION

