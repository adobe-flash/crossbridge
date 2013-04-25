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
$PYTHON_RUNTESTS ./runsmokes.py --testfile=./misc_bug_testing.txt --time=0
ret=$?

exitcode=0
test "$ret" = "0" ||
   exitcode=1

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

exit $exitcode


