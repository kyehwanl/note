#! /bin/bash -x

# download vim files
wget https://github.com/kyehwanl/config_ready_to_use/raw/master/.vimrc
wget https://github.com/kyehwanl/config_ready_to_use/raw/master/_.vim_2023_0203.tar.gz 
tar xvfz _.vim_2023_0203.tar.gz 


# time zone change
sudo timedatectl set-timezone America/New_York
sudo apt install ctags cscope -y


# ENV variables
alias pst="pstree -hup | grep `echo $$`"
alias qq="exit"
echo 'alias pst="pstree -hup | grep `echo $$`"' >> ~/.bashrc
echo 'alias qq="exit"' >> ~/.bashrc


