#!/bin/bash -x

YUM_CMD=$(which yum)
APT_GET_CMD=$(which apt)

PACKAGE=bash-completion
YUM_PACKAGE_NAME=$PACKAGE
DEB_PACKAGE_NAME=$PACKAGE


if [ -f /etc/profile.d/bash_completion.sh ]; then # in case not exist, apt or yum install bash-completion
  source /etc/profile.d/bash_completion.sh # this line is for the case of errors, "bash: _get_comp_words_by_ref: command not found"
  source <(kubectl completion bash)
  alias k=kubectl
  source <(kubectl completion bash | sed s/kubectl/k/g)
else

	if [[ ! -z $YUM_CMD ]]; then
		yum install -y $YUM_PACKAGE_NAME
	elif [[ ! -z $APT_GET_CMD ]]; then
		apt install -y $DEB_PACKAGE_NAME
	else
		echo "error can't install package $PACKAGE"
		exit 1;
	fi
  source /etc/profile.d/bash_completion.sh # this line is for the case of errors, "bash: _get_comp_words_by_ref: command not found"
  source <(kubectl completion bash)
  alias k=kubectl
  source <(kubectl completion bash | sed s/kubectl/k/g)
fi
