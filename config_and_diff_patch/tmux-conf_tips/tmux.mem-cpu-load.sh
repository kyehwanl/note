#!/bin/bash -x
# 
PATH=/users/kyehwanl/Download/tmux-2.0/tmux
#----------------------------------------------
# CPU, RAM, and load monitor for use with tmux
#----------------------------------------------
# - source link for cpu load (https://github.com/thewtex/tmux-mem-cpu-load, https://travis-ci.org/thewtex/tmux-mem-cpu-load) 
# - put this executable binary(~/workspace/tmux-mem-cpu-load) into the system path, such as  /bin/ or /usr/local/bin
#
$PATH set-option -g status-right-length 140
$PATH set-option -g status-right "#[fg=white,bg=default,bright]#(tmux-mem-cpu-load --interval 2) "
#$PATH set-option -g status-right "#[fg=white,bg=default,bright]#(tmux-mem-cpu-load --colors --interval 2) "
$PATH set-option -ga status-right "#22T %H:%M %d-%b-%y"
# or 
#$PATH set-option -ag status-right " #[fg=white,bg=default]%a%l:%M:%S %p#[default] #[fg=blue]%Y-%m-%d"


#  Example output:
#
#  2885/7987MB [|||||     ]  51.2% 2.11 2.35 2.44
#
#   ^    ^          ^         ^     ^    ^    ^
#   |    |          |         |     |    |    |
#   1    2          3         4     5    6    7
#
#  1. Currently used memory.
#  2. Available memory.
#  3. CPU usage bar graph.
#  4. CPU usage percentage.
#  5. Load average for the past minute.
#  6. Load average for the past 5 minutes.
#  7. Load average for the past 15 minutes.
#  For terminals with 256 color support, graded colors can be displayed by passing the --colors flag.




