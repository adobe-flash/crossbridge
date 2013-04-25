#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1


showhelp ()
{
    echo ""
    echo "usage: run-selftest-generic.sh <change> <filename>"
    echo "  <change>     build number of shell to test"
    echo "  <filename>   name of the shell, do not include file extenstion"
    exit 1
}

if [ "$#" -lt 2 ]
then
    echo "not enough args passed"
    showhelp
fi

filename=$2

export shell=$filename$shell_extension

# If running under Jenkins, avm and asc come from upstream jobs via the
# "copy artifact" plugin and should not be downloaded via ftp
if [ "$JENKINS_HOME" = "" ]; then
    ##
    # Download the AVMSHELL if it does not exist
    ##
    download_shell $shell

    export native_shell=${buildsdir}/${change}-${changeid}/${platform}/$shell

fi # end Jenkins check

## 
# call ssh-shell-deployer.sh to make sure the remote device is setup
##
echo "setup ${native_shell}"
../all/ssh-shell-deployer.sh ${change} ${native_shell}
res=$?
test "$res" = "0" || {
    echo "message: setup failed"
    exit 1
}


###
# check for running avmshell processes
###
echo "checking for running avmshell processes"
../all/util-process-clean-ssh.sh


export AVM=$basedir/build/buildbot/slaves/all/ssh-shell.sh

echo AVM=$AVM
echo "`$AVM`"
test -f $AVM || {
  echo "ERROR: $AVM not found"
  exit 1
}
echo; echo "AVM built with the following options:"
echo "`$AVM -Dversion`"
echo ""

$AVM -Dselftest > selftest.out 2>&1
ret=$?

cat selftest.out

passes=`grep pass selftest.out | wc -l`
fails=`grep fail selftest.out | wc -l`

# a non-zero exit code indicates an avm crash, therefore increment the failures by 1
if [ "$ret" -ne "0" ]; then
    let fails=fails+1
fi

echo "passes            : $passes"
echo "failures          : $fails"

# If running in Jenkins, don't delete this file yet, still need
# to generate a junit report based on it
if [ "$JENKINS_HOME" = "" ]; then
    rm selftest.out
fi

###
# check for running avmshell processes
###
 echo "checking for running avmshell processes"
 cd $basedir/build/buildbot/slaves/scripts
 ../all/util-process-clean-ssh.sh

exit $ret
 
