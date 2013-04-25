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


# silence output if silent=true (function defined in environment.sh)
slavename=`cd ${basedir}/..; pwd`
slavename=${slavename##*/}
logfile=smokes-${slavename}.log
beginSilent

configfile=$2
test "$configfile" = "" && {
    configfile=./runsmokes.txt
}


##
# Download the shell_release
##
download_shell $shell_release

##
# Download the shell_release_debugger
##
download_shell $shell_release_debugger

##
# Download the shell_debug
##
download_shell $shell_debug

##
# Download the shell_debug_debugger
##
download_shell $shell_debug_debugger


##
# Download the latest asc.jar if it does not exist
##
download_asc

echo ""
echo "Missing media will be compiled using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

# set variables for full path to builds
export avmr=$buildsdir/$change-${changeid}/$platform/$shell_release
export avmrd=$buildsdir/$change-${changeid}/$platform/$shell_release_debugger
export avmd=$buildsdir/$change-${changeid}/$platform/$shell_debug
export avmdd=$buildsdir/$change-${changeid}/$platform/$shell_debug_debugger
# cmdline testsuite uses AVM,AVMRD, should change to avmr,avmrd
export AVM=$avmr
export AVMRD=$avmrd

##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh

cd $basedir/test
$PYTHON_RUNTESTS ./runsmokes.py --testfile="$configfile" --time=120
ret=$?

exitcode=0
test "$ret" = "0" ||
   exitcode=1

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

endSilent

test "$silent" = "true" && {
    # display smoke results to stdout so that buildbot parses the results
    grep "^ *passes" $logfile
    grep "^ *failures" $logfile
}

exit $exitcode


