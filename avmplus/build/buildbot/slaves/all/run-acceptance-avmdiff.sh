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

download_shell $shell_release_debugger

download_shell $shell_debug

download_shell $shell_debug_debugger

##
# Download the latest asc.jar if it does not exist
##
download_asc

echo ""
echo "Missing media will be compiled using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

export shell_release=$buildsdir/$change-${changeid}/$platform/$shell_release
export shell_release_debugger=$buildsdir/$change-${changeid}/$platform/$shell_release_debugger
export shell_debug=$buildsdir/$change-${changeid}/$platform/$shell_debug
export shell_debug_debugger=$buildsdir/$change-${changeid}/$platform/$shell_debug_debugger



##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh


cd $basedir/test/acceptance

avmdiff=$basedir/utils/avmdiff.py
export AVM="$PYTHON_RUNTESTS $avmdiff"
echo "`$AVM`"
echo; echo "AVM built with the following options:"
echo "`$AVM -Dversion`"


echo "message: $PYTHON_RUNTESTS ./runtests.py --config=${platform}-diff   --notimecheck"
$PYTHON_RUNTESTS ./runtests.py --config=${platform}-diff   --notimecheck

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

exit 0


