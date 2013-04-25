#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

revision=$1
shell=$2
args=$3

test "$shell" = "" && shell=$shell_release_debugger
test "$args" = "" && args="--quiet"

if [ "$shell" = "avmshell_sd" -o "$shell" = "avmshell_sd_64" ]
then
    knownerrors=knownerrors-debug.txt
else
    knownerrors=knownerrors.txt
fi

# calculate the revision number to test
. ../all/util-calculate-change.sh $revision

# download build
download_shell $shell

# set the release debugger shell
export AVMRD=$buildsdir/$change-${changeid}/$platform/$shell

test "$BRIGHTSPOT" = "" && {
    echo "BRIGHTSPOT must be set to the url of brightspot"
    exit 1
}

# download the avmglue.abc, requiring for running swfs in vm
echo "Download avmglue.abc"
../all/util-download.sh $BRIGHTSPOT/avmglue.abc $basedir/test/brightspot/avmglue.abc
echo "Download runtests.py"
../all/util-download.sh $BRIGHTSPOT/runtests.py $basedir/test/brightspot/runtests.py
echo "Download knownerrors.txt"
../all/util-download.sh $BRIGHTSPOT/$knownerrors $basedir/test/brightspot/knownerrors.txt

cd $basedir/test/brightspot
chmod +x ./runtests.py

# run the testsuite
$PYTHON_RUNTESTS ./runtests.py $args

