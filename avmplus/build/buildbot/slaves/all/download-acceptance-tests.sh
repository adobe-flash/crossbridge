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

cd $basedir/test/acceptance
echo "Removing old *.abc files"
find ./ -name "*.abc" -exec rm {} \;
echo "Revert any changes in the acceptance directory."
echo "This will allow .abc files to actually be checked in."
hg revert ./

## download acceptance test abc zip
echo "Download acceptance-tests-abcs.zip"
$basedir/build/buildbot/slaves/all/util-download.sh $ftp_asteam/builds/$branch/$change-${changeid}/acceptance-tests-abcs.zip $basedir/test/acceptance/acceptance-tests-abcs.zip 
res=$?

if [ "$res" = "0" -a -f "$basedir/test/acceptance/acceptance-tests-abcs.zip" ]
then

echo "unzipping acceptance tests"
unzip -o -q acceptance-tests-abcs.zip

else

echo "download acceptance-tests-abcs.zip failed, rebuilding tests" 

##
# Download the latest asc.jar if it does not exist
##
download_asc

echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

echo "Rebuilding ABC test media"

if [[ $AVM == *ceremoteshell* ]]
then
    python ./runtests.py --rebuildtests -c arm-winmobile-tvm-release-arm 
else
    python ./runtests.py --rebuildtests 
fi

fi # manually rebuild tests


# clean up
rm $basedir/test/acceptance/acceptance-tests-abcs.zip
rm $basedir/test/acceptance/download.log
echo "finished"

exit 0

