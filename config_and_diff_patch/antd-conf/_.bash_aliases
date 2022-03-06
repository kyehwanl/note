# Some useful aliases.
alias h='history'
alias j="jobs -l"
alias l="ls -l "
alias ll="ls -htrl"
alias ls="ls -F --color=auto"
alias vi='vim'
alias lll='less'
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'
alias grep='grep --color=auto'
alias pst='pstree -hup | grep `echo $$`'
alias vifm='source ~/vf'
#alias vim='vim -T xterm-256color'
alias dl='docker ps -l -q'
alias gl='git lgodb'
alias gs='git status'
alias gd='git diff -w -b'
alias qq='exit'
alias docker='sudo docker'


# Function which adds an alias to the current shell and to
# the ~/.bash_aliases file.
add-alias ()
{
   local name=$1 value="$2"
   echo alias $name=\'$value\' >>~/.bash_aliases
   eval alias $name=\'$value\'
   alias $name
}

# "repeat" command.  Like:
#
#	repeat 10 echo foo
repeat ()
{ 
    local count="$1" i;
    shift;
    for i in $(seq 1 "$count");
    do
        eval "$@";
    done
}

# Subfunction needed by `repeat'.
seq ()
{ 
    local lower upper output;
    lower=$1 upper=$2;
    while [ $lower -le $upper ];
    do
        output="$output $lower";
        lower=$[ $lower + 1 ];
    done;
    echo $output
}

