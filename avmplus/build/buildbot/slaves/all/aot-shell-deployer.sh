# -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- #
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) #
#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

# threadcount is the number of child threads
# count is used to loop over the threads
if [ "$threads" == "" ]
then
   threadcount="1"
else
   threadcount=$threads
fi
count="0"

# keep track of any failures
exitcode=0


# loop through each ssh client: for count=1 to threadcount, count contains the current client
while true
do
    # set SSH_SHELL_REMOTE_* to the current ssh client
    eval SSH_SHELL_REMOTE_USER=\${SSH_SHELL_REMOTE_USER$count}
    eval SSH_SHELL_REMOTE_HOST=\${SSH_SHELL_REMOTE_HOST$count}
    eval SSH_SHELL_REMOTE_BASEDIR=\${SSH_SHELL_REMOTE_BASEDIR$count}
    eval SSH_SHELL_REMOTE_DIR=\${SSH_SHELL_REMOTE_DIR$count}

    if [ "$SSH_SHELL_REMOTE_USER" = "" ] ||
       [ "$SSH_SHELL_REMOTE_HOST" = "" ] ||
       [ "$SSH_SHELL_REMOTE_BASEDIR" = "" ] ||
       [ "$SSH_SHELL_REMOTE_DIR" = "" ];
    then
        echo "missing environment variable: "
        echo "SSH_SHELL_REMOTE_USER${count}" = "$SSH_SHELL_REMOTE_USER"
        echo "SSH_SHELL_REMOTE_HOST${count}" = "$SSH_SHELL_REMOTE_HOST"
        echo "SSH_SHELL_REMOTE_BASEDIR${count}" = "$SSH_SHELL_REMOTE_BASEDIR"
        echo "SSH_SHELL_REMOTE_DIR${count}" = "$SSH_SHELL_REMOTE_DIR"
        exit 1
    fi

    echo "setting up client $count"
    echo "SSH_SHELL_REMOTE_USER" = "$SSH_SHELL_REMOTE_USER"
    echo "SSH_SHELL_REMOTE_HOST" = "$SSH_SHELL_REMOTE_HOST"
    echo "SSH_SHELL_REMOTE_BASEDIR" = "$SSH_SHELL_REMOTE_BASEDIR"
    echo "SSH_SHELL_REMOTE_DIR" = "$SSH_SHELL_REMOTE_DIR"

    # Create the builds directory, this is not used BUT IS NECESSARY for the
    # utils-acceptance-clean-ssh.sh script since it tries to clean the dir
    ssh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST "${SSH_SETUP}mkdir -p $SSH_SHELL_REMOTE_BASEDIR/builds"

    # copy the aot-shell-runner.sh to the client
    echo "Installing aot-shell-runner.sh on $SSH_SHELL_REMOTE_HOST to $SSH_SHELL_REMOTE_DIR"
    ssh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST "${SSH_SETUP}mkdir -p $SSH_SHELL_REMOTE_DIR;cd $SSH_SHELL_REMOTE_DIR;rm aot-shell-runner.sh"
    scp ../all/aot-shell-runner.sh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST:$SSH_SHELL_REMOTE_DIR/aot-shell-runner.sh
    # confirm the copy succeeded
    if [[ "$?" -ne "0" ]]; then
        echo "Error copying aot-shell-runner.sh."
        # If there was an error copying the script abort
        exit 1
    fi
    # set executable bit for aot-shell-runner.sh
    ssh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST "cd $SSH_SHELL_REMOTE_DIR;chmod 777 aot-shell-runner.sh"

    # increment the counter, check if we are finished looping
    count=$[count+1]
    if [ "$count" = "$threadcount" ]
    then
        break
    fi
done

exit $exitcode
