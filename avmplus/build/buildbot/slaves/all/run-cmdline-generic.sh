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
    echo "usage: run-cmdline-generic.sh <change> <filename>"
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


##
# Download the AVMSHELL if it does not exist
##
download_shell $shell


##
# Download the latest asc.jar if it does not exist
##
download_asc

##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh


export AVM=$buildsdir/$change-${changeid}/$platform/$shell
echo AVM=$AVM
echo "`$AVM`"
test -f $AVM || {
  echo "ERROR: $AVM not found"
  exit 1
}
echo; echo "AVM built with the following options:"
echo "`$AVM -Dversion`"
echo ""

if [ "`$AVM -Dversion|grep AVMFEATURE_DEBUGGER`" != "" ];
then
    echo "Running a shell with a debugger, set AVMRD"
    export AVMRD=$AVM
fi

cd $basedir/test/cmdline
echo "starting cmdline testsuite..."


$PYTHON_RUNTESTS ./runtests.py > cmdline.txt 2>&1
ret=$?
echo "ret=$ret"

cat cmdline.txt

pass=`cat cmdline.txt | grep PASSED | wc -l | awk '{print $1}'`
fails=`cat cmdline.txt | grep FAILED | wc -l | awk '{print $1}'`
exfails=`cat cmdline.txt | grep "EXPECTED FAIL" | wc -l | awk '{print $1}'`
unpass=`cat cmdline.txt | grep "UNEXPECTED" | wc -l | awk '{print $1}'`
skips=`cat cmdline.txt | grep "SKIP" | wc -l | awk '{print $1}'`

echo "Summary results:"
echo "passes            : $pass"
echo "failures          : $fails"
test "$exfails" = "0" || {
echo "expected failures : $exfails"
}
test "$unpass" = "0" || {
echo "unexpected passes : $unpass"
}
test "$skips" = "0" || {
echo "tests skipped     : $skips"
}

rm cmdline.txt

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

exit $ret
