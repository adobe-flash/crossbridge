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
# Download the latest asc.jar if it does not exist
##
download_asc

echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""


cd $basedir/test/acceptance
echo "Removing old *.abc files"
find ./ -name "*.abc" -exec rm {} \;
echo "Revert any changes in the acceptance directory."
echo "This will allow .abc files to actually be checked in."
hg revert ./

echo "Rebuilding ABC test media"
python ./runtests.py --rebuildtests
python ./runtests.py --rebuildtests --config="deep"

# create zip of .abc files
files=`find . -name "*.abc*"`
zip -q -r acceptance-tests-abcs.zip $files

exit 0

