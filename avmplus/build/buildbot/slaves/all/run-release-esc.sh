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



export scriptsdir=`pwd`
echo scriptsdir: $scriptsdir

# If running under Jenkins, avm and asc come from upstream jobs via the
# "copy artifact" plugin and should not be downloaded via ftp
if [ "$JENKINS_HOME" = "" ]; then
    ##
    # Download the AVMSHELL if it does not exist
    ##
    download_shell $shell_release

    chmod +x $buildsdir/$change-${changeid}/$platform/$shell_release

    echo ""
    cp $buildsdir/$change-${changeid}/$platform/$shell_release $basedir/esc/bin/shell${shell_extension}
else
    cp $basedir/objdir/shell/$shell_release $basedir/esc/bin/shell${shell_extension}
fi


export AVM=$basedir/esc/bin/shell${shell_extension}
chmod +x $AVM

cd $basedir/esc/build
make clean

##
# First compilation of ESC
##
echo; echo "First compilation of ESC"; echo
make
ret=$?
test "$ret" = "0" || {
  echo "esc initial build failed..."
  exit 1
}
make clean




##
# Second compilation of ESC
##
echo; echo "Second compilation of ESC"; echo
make
ret=$?
test "$ret" = "0" || {
  echo "esc 2nd build failed..."
  exit 1
}
make clean



##
# Third compilation of ESC
##
echo; echo "Third compilation of ESC"; echo
make
ret=$?
test "$ret" = "0" || {
  echo "esc 3rd build failed..."
  exit 1
}
make clean



##
# Fourth compilation of ESC
##
echo; echo "Fourth compilation of ESC"; echo
make
ret=$?
test "$ret" = "0" || {
  echo "esc 4th build failed..."
  exit 1
}
make clean

