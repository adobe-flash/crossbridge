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



##
# Download the AVMSHELL if it does not exist
##
download_shell $shell_release


echo "setup $branch/${change}-${changeid}"
../all/ssh-shell-deployer.sh ${change} ${buildsdir}/${change}-${changeid}/${platform}/$shell_release
res=$?
test "$res" = "0" || {
    echo "message: setup failed"
    exit 1
}

download_asc

export AVM=$basedir/build/buildbot/slaves/all/ssh-shell.sh
export avmr=$AVM
export avmrd=$AVM
export avmd=$AVM
export avmdd=$AVM

# silence output if silent=true (function defined in environment.sh)
logfile=smokes-mips.log
beginSilent

cd $basedir/test
$PYTHON_RUNTESTS ./runsmokes.py --testfile=./runsmokes-arm-mips.txt --time=120
ret=$?

exitcode=0
test "$ret" = "0" ||
   exitcode=1

endSilent

test "$silent" = "true" && {
    # display smoke results to stdout so that buildbot parses the results
    grep "^ *passes" $logfile
    grep "^ *failures" $logfile
}

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

exit $exitcode