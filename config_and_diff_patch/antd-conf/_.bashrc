# Bourne Again SHell init file.
#
# Files you make look like rw-rw-r
umask 027

# Don't make useless coredump files.  If you want a coredump,
# say "ulimit -c unlimited" and then cause a segmentation fault.
ulimit -c 0

alias unsetenv=unset
function setenv () {
  export $1="$2"
}

setenv TEXINPUTS .:/usr/local/lib/tex/inputs.2e:/usr/local/lib/tex/inputs
setenv OPENWINHOME /usr/openwin
setenv MANPATH ~/man:/opt/java.1.3/man:/usr/share/man:/import/fs1/local/man:/usr/contrib/man::~/test/usr/local/man:$OPENWINHOME/man:/usr/local/man:/usr/man:/usr/X11/man:/$OPENWINHOME/guide/man:/usr/snm/man

#setenv DISPLAY :0.0
setenv LD_LIBRARY_PATH=/usr/openwin/lib:/opt/GCC281/lib/gcc-lib/sparc-sun-solaris2.6/2.8.1:/opt/GCC281/lib:/usr/local/lib:/import/fs1/local/lib:/import/fs1/local/solaris/lib:/import/fs1/local/fs1-usr-lib
setenv FONTPATH $OPENWINHOME/lib/fonts:/usr/X11/lib/fonts:/usr/local/lib/X11/fonts:/usr/local/src/lexmark/fonts:/usr/lib/fonts/fixedwidthfonts:/usr/lib/fonts/tekfonts
setenv ow  $OPENWINHOME
setenv NAME ''
setenv TROFF ptroff
OOP_PATH=.:/opt/GCC281/bin:/usr/ccs/bin
OPENWIN_PATH=$OPENWINHOME/bin:/usr/openwin/bin/xview:/usr/openwin/bin
SYSTEM_PATH=/opt/java.1.3/bin:/usr/bin:/usr/ccs/bin:/import/fs1/local/bin:/import/fs1/local/solaris/bin:/import/fs1/local/solaris/netscape:/opt/JBxv/bin:/bin:/usr/local/gnu/bin:/usr/bin:/usr/ucb:/usr/local/bin:/usr/etc:/usr/local/etc:/usr/games:/opt/AEgs/bin:/opt/GNUgview/bin:/import/fs1/opt/Acrobat3/bin:/import/fs1/opt/xfig.3.2.2:/usr/jdk1.2/bin:/opt/kde/bin:/usr/local/sbin
MOUNT_PATH=/itl/links/generic/bin:/itl/links/gnu/bin:/itl/links/spro/bin
MAN2_PATH=/itl/apps/generic/man:/itl/apps/gnu/bin:/itl/apps/spro/bin
TEMP_PATH=/itl/apps/tetex/SunOS-sun4m-5.6/bin
#JAVA_PATH=/usr/local/jdk_150/bin
#JAVA_PATH=/users/www/local/terrain_aps/jdk1.6.0_23/

STANDARD_PATH=$LOCAL_PATH:$OPENWIN_PATH:$SYSTEM_PATH:$SUNLINK_PATH:$SPECIAL_PATH:$MOUNT_PATH:$TEMP_PATH:$MAN2_PATH:$JAVA_PATH
#RPKI_UTIL_PATH=/users/kyehwanl/subvert-rpki.hactrn.net/utils
PYCSCOPE_PATH=/users/kyehwanl/pycscope-0.3/build/scripts-2.5/
#RPKI_TS2=/users/kyehwanl/subvert-rpki.hactrn.net/rpkid
#RPKI_TS1=/users/kyehwanl/rpki_link/rpkid
#RPKI_TE1=/users/kyehwanl/ter1_rpki/rpkid
#RPKI_TE2=/users/kyehwanl/ter2_rpki/rpkid
#export RPKI_TS2
#export RPKI_TS1
#export RPKI_TE1
#export RPKI_TE2

PATH=$JAVA_PATH/bin/:$OOP_PATH:$STANDARD_PATH:/opt/gnu/bin:/usr/sbin:/sbin:/usr/lib:/usr/X11R6/bin:$RPKI_UTIL_PATH:$PYCSCOPE_PATH

# If running interactively, then:
if [ "$PS1" ]; then
  ignoreeof=
  no_exit_on_failed_exec=
fi

prefix=`hostname | sed -e 's/\./-/' -e 's/\..*//' -e 's/-/\./'`
suffix="-> "

function set_prompt() {
  #PS1="$prefix [\!]{`dirs|sed -e 's| .*||' -e 's|.*[^/]\(/[^/]*/[^/]*\)|...\1|'`}$suffix" 
  #PS1="\e[01;32m\h\e[m\e[01;38m [\!]{`dirs|sed -e 's| .*||' -e 's|.*[^/]\(/[^/]*/[^/]*\)|...\1|'`}\e[m\$ "
  PS1="\[\033[01;32m\]\h\[\033[00m\]\[\033[01;38m\] [\!]{`dirs|sed -e 's| .*||' -e 's|.*[^/]\(/[^/]*/[^/]*\)|...\1|'`}\[\033[00m\]\$ "
}

PROMPT_COMMAND=set_prompt
HISTSIZE=2000
HISTFILE=~/.history
#HISTFILESIZE=50
export HISTCONTROL=ignoredups
export HISTIGNORE="pwd:ls:ls -htrl:ll"

if [ -f ~/.bash_aliases ]; then
  source ~/.bash_aliases
fi

# --- Original color setting
#LS_COLORS='di=01;34:*.tar=01;31:*.tgz=01;31:*.arj=01;31:*.taz=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=01;31:*.lz=01;31:*.xz=01;31:*.bz2=01;31:*.tbz=01;31:*.tbz2=01;31:*.bz=01;31:*.tz=01;31'

LS_COLORS='di=38;5;27:*.tar=01;31:*.tgz=01;31:*.arj=01;31:*.taz=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=01;31:*.lz=01;31:*.xz=01;31:*.bz2=01;31:*.tbz=01;31:*.tbz2=01;31:*.bz=01;31:*.tz=01;31:*.rpm=01;31:*.c=33:*Makefile=95:*.am=35:*.o=90:*.ac=96:*.h=90'
#LS_COLORS+='rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:mi=01;05;37;41:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arj=01;31:*.taz=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=01;31:*.lz=01;31:*.xz=01;31:*.bz2=01;31:*.tbz=01;31:*.tbz2=01;31:*.bz=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.jpg=01;35:*.jpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.axv=01;35:*.anx=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=01;36:*.au=01;36:*.flac=01;36:*.mid=01;36:*.midi=01;36:*.mka=01;36:*.mp3=01;36:*.mpc=01;36:*.ogg=01;36:*.ra=01;36:*.wav=01;36:*.axa=01;36:*.oga=01;36:*.spx=01;36:*.xspf=01;36:'

# ---Emulab color setting below
#rs=0:di=38;5;27:ln=38;5;51:mh=44;38;5;15:pi=40;38;5;11:so=38;5;13:do=38;5;5:bd=48;5;232;38;5;11:cd=48;5;232;38;5;3:or=48;5;232;38;5;9:mi=05;48;5;232;38;5;15:su=48;5;196;38;5;15:sg=48;5;11;38;5;16:ca=48;5;196;38;5;226:tw=48;5;10;38;5;16:ow=48;5;10;38;5;21:st=48;5;21;38;5;15:ex=38;5;34:*.tar=38;5;9:*.tgz=38;5;9:*.arj=38;5;9:*.taz=38;5;9:*.lzh=38;5;9:*.lzma=38;5;9:*.tlz=38;5;9:*.txz=38;5;9:*.zip=38;5;9:*.z=38;5;9:*.Z=38;5;9:*.dz=38;5;9:*.gz=38;5;9:*.lz=38;5;9:*.xz=38;5;9:*.bz2=38;5;9:*.tbz=38;5;9:*.tbz2=38;5;9:*.bz=38;5;9:*.tz=38;5;9:*.deb=38;5;9:*.rpm=38;5;9:*.jar=38;5;9:*.war=38;5;9:*.ear=38;5;9:*.sar=38;5;9:*.rar=38;5;9:*.ace=38;5;9:*.zoo=38;5;9:*.cpio=38;5;9:*.7z=38;5;9:*.rz=38;5;9:*.jpg=38;5;13:*.jpeg=38;5;13:*.gif=38;5;13:*.bmp=38;5;13:*.pbm=38;5;13:*.pgm=38;5;13:*.ppm=38;5;13:*.tga=38;5;13:*.xbm=38;5;13:*.xpm=38;5;13:*.tif=38;5;13:*.tiff=38;5;13:*.png=38;5;13:*.svg=38;5;13:*.svgz=38;5;13:*.mng=38;5;13:*.pcx=38;5;13:*.mov=38;5;13:*.mpg=38;5;13:*.mpeg=38;5;13:*.m2v=38;5;13:*.mkv=38;5;13:*.ogm=38;5;13:*.mp4=38;5;13:*.m4v=38;5;13:*.mp4v=38;5;13:*.vob=38;5;13:*.qt=38;5;13:*.nuv=38;5;13:*.wmv=38;5;13:*.asf=38;5;13:*.rm=38;5;13:*.rmvb=38;5;13:*.flc=38;5;13:*.avi=38;5;13:*.fli=38;5;13:*.flv=38;5;13:*.gl=38;5;13:*.dl=38;5;13:*.xcf=38;5;13:*.xwd=38;5;13:*.yuv=38;5;13:*.cgm=38;5;13:*.emf=38;5;13:*.axv=38;5;13:*.anx=38;5;13:*.ogv=38;5;13:*.ogx=38;5;13:*.aac=38;5;45:*.au=38;5;45:*.flac=38;5;45:*.mid=38;5;45:*.midi=38;5;45:*.mka=38;5;45:*.mp3=38;5;45:*.mpc=38;5;45:*.ogg=38;5;45:*.ra=38;5;45:*.wav=38;5;45:*.axa=38;5;45:*.oga=38;5;45:*.spx=38;5;45:*.xspf=38;5;45:

#use 'ctrl shit -' to replace 'ctrl s'
#stty stop  

#alias remN='Xnest :1 -ac -geometry 1024x700 &'
#alias remG='gnome-session --display :1 &'
#alias remX='remN remG'

#export DISPLAY=:11.0

export TERM=xterm-256color   # without this setting, shell file coloring becomes weird
#export TERM=screen-256color
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/users/kyehwanl/Download/libevent-2.0.21-stable/_inst/usr/local/lib"

if [ -f ~/.go ]; then
    source ~/.go
fi

export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.181-3.b13.el7_5.x86_64/jre



docker-ip() {
  sudo docker inspect --format '{{ .NetworkSettings.IPAddress }}' "$@"
}

# Created by `userpath` on 2021-10-25 04:45:30
export PATH="$PATH:/users/kyehwanl/.local/bin"

# --- Kubernetes Setting ----
if [ -f ~/.kubectl_bash_completion ]; then
  if command -v kubectl &> /dev/null
  then
    echo "kubectl found"
    source ~/.kubectl_bash_completion
  fi
fi
#export KUBECONFIG=$HOME/.kube/config2
#source /etc/profile.d/bash_completion.sh
#source <(kubectl completion bash)
#alias k=kubectl
#source <(kubectl completion bash | sed s/kubectl/k/g)







