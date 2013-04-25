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


# If running under Jenkins, avm and asc come from upstream jobs via the
# "copy artifact" plugin and should not be downloaded via ftp
if [ "$JENKINS_HOME" = "" ]; then
    ##
    # Download the AVMSHELL if it does not exist
    ##
    download_shell $shell_release_32
    download_shell $shell_release_64

    ##
    # Download the latest asc.jar if it does not exist
    ##
    download_asc

    ## HACK, use the release and debug variables for the 32 and 64 bit shells
    export shell_release=$buildsdir/$change-${changeid}/$platform/$shell_release_32
    export shell_debug=$buildsdir/$change-${changeid}/$platform/$shell_release_64

else
    ## HACK, use the release and debug variables for the 32 and 64 bit shells
    export shell_release=$WS/objdir/shell/$shell_release_32
    export shell_debug=$WS/objdir/shell/$shell_release_64
fi

echo ""
echo "Missing media will be compiled using the following ASC version:"
echo "`java -jar $ASC`"
echo ""





##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh


cd $basedir/test/acceptance

avmdiff=$basedir/utils/avmdiff.py
avmdiffcfg=$basedir/build/buildbot/slaves/all/avmdiff_32-64.cfg
if [ "$OSTYPE" == "cygwin" ]; then
    avmdiff=`cygpath -w $basedir/utils/avmdiff.py`
fi
export AVM="$PYTHON_RUNTESTS $avmdiff --buildfile=${avmdiffcfg}"
echo "`$AVM`"
echo; echo "AVM built with the following options:"
echo "`$AVM -Dversion`"


echo "message: $PYTHON_RUNTESTS ./runtests.py --config=${platform}-3264-diff   --notimecheck"
$PYTHON_RUNTESTS ./runtests.py --config=${platform}-3264-diff --notimecheck

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

exit 0


