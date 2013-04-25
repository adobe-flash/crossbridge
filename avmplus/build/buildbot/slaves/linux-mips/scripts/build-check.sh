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



fail=0


# Release_mips-linux
test -f $buildsdir/$change-${changeid}/$platform/$shell_release || {
  echo "message: Release_mips-linux Failed"
  fail=1
}

# Debug_mips-linux
test -f $buildsdir/$change-${changeid}/$platform/$shell_debug || {
  echo "message: Debug_mips-linux Failed"
  fail=1
}

if test "${fail}" = 1; then
   echo Failing the build
   exit 1
fi





