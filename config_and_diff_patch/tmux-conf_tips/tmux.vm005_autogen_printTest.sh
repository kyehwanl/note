#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="vm005"

tmux -2 new-session -d -n "{gobgp|exabgp}|srx vim-session" -s "$SESSION"
# -2: Force tmux to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]

# setup window
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "/opt/project/TestBgpsecImple.git/netns_peer_gen" 
sleep 1
tmux send-keys "cd /opt/project/gobgp_test/gobgp_v2_running_test/bin" 
tmux send-keys "sudo ./gobgpd -f gobgpd.ns1.conf.bgpsec -p -l info" 


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 1
tmux send-keys "cd /opt/project/exabgp_test/exabgp-4.1.2" C-m
tmux send-keys "export PYTHONPATH=/opt/project/exabgp_test/exabgp-4.1.2/lib && python /opt/project/exabgp_test/exabgp-4.1.2/lib/exabgp/application/bgp.py /opt/project/exabgp_test/exabgp-4.1.2/bgpd.node1.conf" 


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 2
tmux send-keys "cd /opt/project/gobgp_test/gobgp_v2_running_test/src/github.com/osrg/gobgp" C-m
tmux send-keys "$GOPATH/bin/gobgp global rib add bgpsec origin igp -a ipv4 25.18.22.0/24 nexthop 10.0.50.5" 



tmux select-pane -t 3
tmux send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1> \"")' 
tmux send-keys "gdb --args /opt/project/Github_usnistgov/NIST-BGP-SRx/_inst/sbin/bgpd -f bgpd.n2.conf.bgpsec" 
tmux send-keys "c" 


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 4
tmux send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1> \"")' 
tmux send-keys "cd /opt/project/srx_test1/_inst/bin" 
tmux send-keys "./srx_server" 



# setup a new window
tmux new-window -t $SESSION:1 -n 'vim-session'  # [-t target-window] [-n window-name]
tmux select-pane -t 0   # -t [arget-pane]
tmux send-keys "vim -S ~/workspace/vim-sessions/qsrx-attr.vim" 



# setup a new window
tmux new-window -t $SESSION:2 -n 'docker_testbed' 
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.2 --name exabgpsec02     -p 1792:179 -v /opt/project/docker_test/bgpsecImples/exabgp.docker.conf.bgpsec:/etc/exabgp.conf     -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys     exabgpsec_src  bash" 
sleep 1
tmux send-keys "env exabgp.daemon.user=root exabgp /etc/exabgp.conf"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 1
tmux send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.3 --name exabgpsec03     -p 1793:179 -v /opt/project/docker_test/bgpsecImples/exabgp.docker.conf.bgpsec2:/etc/exabgp.conf     -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys     exabgpsec_src  bash" 
sleep 1
tmux send-keys "env exabgp.daemon.user=root exabgp /etc/exabgp.conf" 


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 2
tmux send-keys "cd /opt/project/Github_usnistgov/NIST-BGP-SRx/_inst/bin" C-m
tmux send-keys "sudo ./rpkirtr_svr -f rpkirtr_svr.conf"


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 3
tmux send-keys "telnet 172.37.0.1 2605" 


tmux split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 4
tmux send-keys "sudo /opt/project/Github_usnistgov/NIST-BGP-SRx/quagga-srx/bgpd/.libs/bgpd -f /opt/project/docker_test/bgpsecImples/bgpd.docker.conf.bgpsec -i /tmp/bgpd.pid" 


tmux select-pane -t 5
tmux send-keys "/opt/project/Github_usnistgov/NIST-BGP-SRx/srx-server/src/srx_server"



# setup a new window
tmux new-window -t $SESSION:3 -n 'db_test' 
tmux split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
tmux select-pane -t 0
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/srx_test1/test_install/Net-Patricia-1.15/libpatricia --select /opt/project/docker_test/bgpsecImples" C-m


tmux select-pane -t 1
tmux send-keys "cd" C-m
tmux send-keys "export TERM=screen-256color" C-m
tmux send-keys "./vf --select /opt/project/Github_usnistgov/NIST-BGP-SRx --select /opt/project/srx_test1/test_install/Net-Patricia-1.15/libpatricia" C-m




# Set default window
tmux select-window -t $SESSION:3    # [-t target-window]

# Attach to session
tmux -2 attach-session -t $SESSION  # [-t target-session]







