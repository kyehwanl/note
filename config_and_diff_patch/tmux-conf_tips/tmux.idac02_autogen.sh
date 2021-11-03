#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
#SESSION=$USER
SESSION="idac02"
TMUX='/users/kyehwanl/Download/tmux-2.0/tmux'

${TMUX} -2 new-session -d -n "docker_test" -s "$SESSION"
# -2: Force ${TMUX} to assume the terminal supports 256 colours
# -d: detach session 
# [-s session-name] [-n window-name]

# setup window
${TMUX} split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
${TMUX} select-pane -t 0
${TMUX} send-keys "sudo -s" C-m
${TMUX} send-keys "cd /opt/project/docker_test/bgpsecImples/" C-m
${TMUX} send-keys "docker-compose rm -fs" C-m
${TMUX} send-keys "docker-compose up" C-m


${TMUX} split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
${TMUX} select-pane -t 1
${TMUX} send-keys "sudo -s" C-m
${TMUX} send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.3 --name gobgpsec -p 1793:179 -v /opt/projt/docker_test/bgpsecImples/gobgpd.docker.conf:/etc/gobgpd.conf -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys gobgpsec_src gobgpd -p -f /etc/gobgpd.conf --log-level=debug" C-m


${TMUX} split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
${TMUX} select-pane -t 2
${TMUX} send-keys "sudo -s" C-m
${TMUX} send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.2 --name exabgpsec -p 1792:179 -v /opt/project/docker_test/bgpsecImples/exabgp.docker.conf:/etc/exabgp.conf -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys exabgpsec_src" C-m



${TMUX} select-pane -t 3
${TMUX} send-keys "sudo -s" C-m
${TMUX} send-keys "/opt/project/TestBgpsecImple/netns_peer_gen" C-m
sleep 1
${TMUX} send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1> \"")' C-m
${TMUX} send-keys "cd /opt/project/TestBgpsecImple" C-m
${TMUX} send-keys "bgpd -f /opt/project/TestBgpsecImple/bgpd.n2.conf.bgpsec -u root -i /tmp/bgpd.pid" C-m


${TMUX} split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
${TMUX} select-pane -t 4
${TMUX} send-keys "sudo -s" C-m
${TMUX} send-keys 'ip netns exec ns1 bash --rcfile <(echo "PS1=\"ns1> \"")' C-m
${TMUX} send-keys "cd /opt/project/TestBgpsecImple/" C-m
${TMUX} send-keys "./srx_server" C-m



${TMUX} split-window -v    # -h: horizontal split [ | ], -v: vertical split [ - ]
${TMUX} select-pane -t 5
${TMUX} send-keys "sudo -s" C-m
${TMUX} send-keys "docker run --rm -it --net bgpsecimples_bgp_net --ip 172.37.0.4 --name quaggasrx -p 1791:179 -p 2605:2605 -v /opt/project/docker_test/bgpsecImples/quaggasrx.docker.conf:/etc/quagga/bgpd.conf -v /opt/project/docker_test/bgpsecImples/keys:/var/lib/bgpsec-keys srxsuite_src bgpd -u root -f /etc/quagga/bgpd.conf" C-m




# setup a new window
${TMUX} new-window -t $SESSION:1 -n 'vifm' 
${TMUX} split-window -h    # -h: horizontal split [ | ], -v: vertical split [ - ]
${TMUX} select-pane -t 0
${TMUX} send-keys "cd" C-m
${TMUX} send-keys "export TERM=screen-256color" C-m
${TMUX} send-keys "./vf --select /opt/project/gobgp_test --select /opt/project/TestBgpsecImple/test_install" C-m


${TMUX} select-pane -t 1
${TMUX} send-keys "cd" C-m
${TMUX} send-keys "export TERM=screen-256color" C-m
${TMUX} send-keys "./vf" C-m



# Set default window
${TMUX} select-window -t $SESSION:1    # [-t target-window]

# Attach to session
${TMUX} -2 attach-session -t $SESSION  # [-t target-session]

# color
${TMUX} set -g status-bg magenta 
${TMUX} set -g status-fg white





