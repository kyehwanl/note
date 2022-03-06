#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
SESSION=$USER

tmux -2 new-session -d -s "$SESSION"

# Setup a window for tailing log files
#tmux new-window -t $SESSION:1 -n 'Logs'
tmux split-window -h
tmux select-pane -t 0
tmux split-window -v
tmux select-pane -t 2
tmux split-window -v
tmux split-window -h
tmux split-window -v

# Set up srx_server and rpki_rtr_server
tmux select-pane -t 4
tmux send-keys "ssh node5.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
sleep 1
tmux send-keys "pkill srx_server" C-m
sleep 1
tmux send-keys "~/srx_test1/test/srx/test_install/bin/srx_server > /tmp/foo1 2>&1 &" C-m
sleep 1
#tmux send-keys "~/proj-bgp/srx/test_install/bin/srx_server > /tmp/foo1 2>&1 &" C-m
tmux send-keys "pkill rpkirtr_svr" C-m
tmux send-keys "rpkirtr_svr" C-m
#tmux send-keys "tail -f /tmp/foo1" C-m

tmux select-pane -t 0
tmux send-keys "ssh node1.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
tmux send-keys "sudo pkill bgpd" C-m
tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -f ~/srx_test1/node1/bgpd.roa.test.n1 -i /tmp/bgpd$$.pid" C-m

tmux select-pane -t 1
tmux send-keys "ssh node2.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
#tmux send-keys "Lee@A218" C-m
tmux send-keys "sudo pkill bgpd" C-m
tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -i /tmp/bgpd$$.pid -f ~/srx_test1/node2/bgpd.roa.test.n2" C-m

tmux select-pane -t 2
tmux send-keys "ssh node3.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
tmux send-keys "sudo pkill bgpd" C-m
tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -f ~/srx_test1/node3/bgpd.roa.test.n3 -i /tmp/bgpd$$.pid" C-m

tmux select-pane -t 3
tmux send-keys "ssh node4.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
tmux send-keys "sudo pkill bgpd" C-m
tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -f ~/srx_test1/node4/bgpd.roa.test.n4 -i /tmp/bgpd$$.pid" C-m

#tmux select-pane -t 4
#tmux send-keys "ssh node5.bs1.bgp-srx.emulab2.antd.nist.gov" C-m
#tmux send-keys "sudo kill -9 `pgrep bgpd`" C-m
#tmux send-keys "sudo ~/proj-bgp/quaggasrx/bgpd/.libs/bgpd -f ~/srx_test1/node5/bgpd.roa.test.n5 -i /tmp/bgpd$$.pid" C-m


# monitoring from node4
tmux select-pane -t 5
sleep 3
tmux send-keys "telnet node3.bs1.bgp-srx.emulab2.antd.nist.gov 2605" C-m
sleep 1
tmux send-keys "z" C-m
sleep 1
tmux send-keys "sh ip bgp" C-m


# Set up layout for all panes
tmux select-layout tiled


# Setup a MySQL window
#tmux new-window -t $SESSION:2 -n 'MySQL' 'mysql -uroot'

# Set default window
tmux select-window -t $SESSION:1

# Attach to session
tmux -2 attach-session -t $SESSION

