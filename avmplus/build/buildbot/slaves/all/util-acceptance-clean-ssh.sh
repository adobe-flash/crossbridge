#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh



echo
echo "+++++++++++++++++++++++++++++++++++++++++"
echo "Ensure that the system is clean and ready"
echo "+++++++++++++++++++++++++++++++++++++++++"
echo

if [ "$threads" == "" ]
then
   threadcount="1"
else
   threadcount=$threads
fi

count="0"

exitcode=0
while true
do
    # use indirect reference to set SSH_SHELL_REMOTE_USERn where n=$count
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

    echo "========================================="
    echo "Cleaning up client $count"
    echo "SSH_SHELL_REMOTE_USER" = "$SSH_SHELL_REMOTE_USER"
    echo "SSH_SHELL_REMOTE_HOST" = "$SSH_SHELL_REMOTE_HOST"
    echo "SSH_SHELL_REMOTE_BASEDIR" = "$SSH_SHELL_REMOTE_BASEDIR"
    echo "SSH_SHELL_REMOTE_DIR" = "$SSH_SHELL_REMOTE_DIR"

    #
    # remove old abc and shells from the device
    #
    echo "cleanup $SSH_SHELL_REMOTE_DIR directory on remote machine..."
    ssh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST "cd $SSH_SHELL_REMOTE_DIR; rm -r ./*"

    echo "cleanup $SSH_SHELL_REMOTE_BASEDIR/builds directory on remote machine..."
    ssh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST "cd $SSH_SHELL_REMOTE_BASEDIR/builds; rm -r ./*"

    echo "cleanup $SSH_SHELL_REMOTE_BASEDIR/acceptance-test-abcs.* on remote machine..."
    ssh $SSH_SHELL_REMOTE_USER@$SSH_SHELL_REMOTE_HOST "cd $SSH_SHELL_REMOTE_BASEDIR; rm -f acceptance-tests-abcs.*" 

    echo "========================================="
    echo ""

    count=$[count+1]
    test "$count" == "$threadcount" && {
        break
    }


done
echo ""

echo "cleanup finished"

echo; echo;


