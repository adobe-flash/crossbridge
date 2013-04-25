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


# Release_mips-linux
../all/util-upload-ftp-asteam.sh $buildsdir/${change}-${changeid}/$platform/$shell_release $ftp_asteam/$branch/$change-${changeid}/$platform/$shell_release
ret=$?
if [ "$ret" != "0" ]; then
    echo "Uploading of $platform/$shell_release failed"
    exit 1
fi

# Debug_mips-linux
../all/util-upload-ftp-asteam.sh $buildsdir/${change}-${changeid}/$platform/$shell_debug $ftp_asteam/$branch/$change-${changeid}/$platform/$shell_debug
ret=$?
if [ "$ret" != "0" ]; then
    echo "Uploading of $platform/$shell_debug failed"
    exit 1
fi


