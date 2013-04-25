#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

. ./environment.sh
. ../all/util-calculate-change.sh $1

##
# Download the AVMSHELL if it does not exist
##
download_shell $shell_release
download_shell $shell_release_debugger
download_shell $shell_debug
download_shell $shell_debug_debugger

##
# Download the latest asc.jar if it does not exist
##
download_asc

echo ASC=$ASC
export AVM=$buildsdir/$change-${changeid}/$platform/$shell_release
export AVMRD=$buildsdir/$change-${changeid}/$platform/$shell_release_debugger


echo AVM=$AVM
echo AVMRD=$AVMRD
echo BUILTINABC=$BUILTINABC
echo SHELLABC=$SHELLABC


##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh

cd $basedir/test/cmdline
echo "starting cmdline testsuite..."

$PYTHON_RUNTESTS ./runtests.py > cmdline.txt 2>&1
ret_release=$?


# run the tests again, this time using debug and debug-debugger shells
# note that we use the release env vars set to the debug players
export AVM=$buildsdir/$change-${changeid}/$platform/$shell_debug
export AVMRD=$buildsdir/$change-${changeid}/$platform/$shell_debug_debugger

# Note that we append to cmdline.txt as we have results from the run above in it
$PYTHON_RUNTESTS ./runtests.py >> cmdline.txt 2>&1
ret_debug=$?

# Boolean OR the two returns to get a combined exitcode (ret) value
ret=$(( $ret_release || $ret_debug ))
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
