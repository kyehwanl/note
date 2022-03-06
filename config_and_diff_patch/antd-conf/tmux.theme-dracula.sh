#!/bin/bash -x
# This script for running on BGPSEC2 server
# 
########################################
# Cosmetics - partially Tmux ver >= 2.0
########################################

# Dracula Colours
#  background_color '#282a36'
#  current_line_color '#44475a'
#  foreground_color '#f8f8f2'
#  comment_color '#6272a4'
#  cyan '#8be9fd'
#  green '#50fa7b'
#  orange '#ffb86c'
#  pink '#ff79c6'
#  purple '#bd93f9'
#  red '#ff5555'
#  yellow '#f1fa8c'

PATH=/users/kyehwanl/Download/tmux-2.0/tmux
# pane border  (Tmux ver >= 2.0)
#
# These two styles tell tmux to make all the boarders dark purple,
# but if my cursor is in a window, make that pane’s borders pink
#
$PATH set -g pane-border-style fg='#6272a4'
$PATH set -g pane-active-border-style fg='#ff79c6'


# message text  (Tmux ver >= 2.0)
#
# with a grey background and blue text.
#
$PATH set -g message-style bg='#44475a',fg='#8be9fd'


# Status Line  (Tmux ver >= 2.0)
#
#  background to a dark grey and the foreground to purple
#
$PATH set -g status-style bg='#44475a',fg='white'


# make the background light grey and the foreground dark grey.
# Then we have a conditional. Has the client prefix key been pressed?
# If so, lets actually make the background pink, if not, don’t do anything. And finally The string ends with ☺
#
# Are we controlling tmux or the content of the panes?
$PATH set -g status-left '#[bg=#f8f8f2]#[fg=#282a36]#{?client_prefix,#[bg=#ff79c6],} ☺ '

# are we zoomed into a pane?
$PATH set -ga status-left '#[bg=#44475a]#[fg=#ff79c6] #{?window_zoomed_flag, ↕ ,   }'



# Window Status
#
# set the default window status to blend in to the rest of the status bar
# and the active window to be purple with white text
#
$PATH set-window-option -g window-status-style fg='#bd93f9',bg=default
$PATH set-window-option -g window-status-current-style fg='#ff79c6',bg='#282a36'



# I wanted to have triangular shapes delineating my windows similar to what Vim Powerline does.
# In order to get the style to work I have to toggle the foreground and background colours before and after the .
# Then to display the name of the window I’m using #I, which is a short-hand for the window_index variable and #W,
# which is short for window_name

$PATH set -g window-status-current-format "#[fg=#44475a]#[bg=#bd93f9]#[fg=#f8f8f2]#[bg=#bd93f9] #I #W #[fg=#bd93f9]#[bg=#44475a]"
$PATH set -g window-status-format "#[fg=#f8f8f2]#[bg=#44475a]#I #W #[fg=#44475a] "

