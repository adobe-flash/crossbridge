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
logfile=build-$platform-$shell_debug.log
beginSilent

##
# Update the version string
##
. ../all/util-update-version.sh

cd $basedir/platform/android
test -f avmshell && {
   rm -f ./avmshell
}
make clean
make MAKECMDGOALS=debug
res=$?


if [ -f ./avmshell ]
then
    test -d $buildsdir/$change-${changeid}/$platform || {
        mkdir -p $buildsdir/$change-${changeid}/$platform
    }
    cp avmshell $buildsdir/$change-${changeid}/$platform/$shell_debug
    endSilent
    exit 0
else
    echo "avmshell was not created"
    endSilent
    exit 1
fi

