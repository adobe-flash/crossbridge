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

# Upload Test media
../all/util-upload-ftp-asteam.sh $basedir/test/acceptance/acceptance-tests-abcs.zip $ftp_asteam/$branch/$change-${changeid}/acceptance-tests-abcs.zip
ret=$?
if [ "$ret" != "0" ]; then
    echo "Uploading of acceptance-tests-abcs.zip failed"
    exit 1
fi
