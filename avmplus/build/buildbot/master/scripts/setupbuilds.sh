#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

branch=$1
buildnum=$2

test "$branch" = "" && {
    echo "You must specify a branch. usage: setupbuilds.sh BRANCH NUMBER"
    exit 0
}

test "$buildnum" = "" && {
    echo "You must specify a build number. usage: setupbuilds.sh BRANCH NUMBER"
    exit 0
}


keeplength="10"
tamarin=/home/ftp/pub/js/tamarin/builds
dir=`ls $tamarin`

# If the directory has already been created, exit out
test -d "$tamarin/$branch/$buildnum" &&
    exit 0

# Clean up the builds, only maintianing the last ${keeplength} number of builds
while true
do
    ls $tamarin/$branch | grep -v latest > build.files
    sz=`wc -l build.files | awk '{print $1}'`
    if [ "$sz" -ge "$keeplength" ]
    then
        last=`cat build.files | sort -n | head -1`
        echo "deleting directory $tamarin/$branch/$last"
        rm -fr $tamarin/$branch/$last
    else
        break
    fi
done


# Since each build slave calls this script, we only need to create the dirs
# if this is the first time calling the script for this build number
test "$buildnum" = "" || {
    echo "creating build directory $branch/$buildnum"
    mkdir $tamarin/$branch/$buildnum
    mkdir $tamarin/$branch/$buildnum/windows
    mkdir $tamarin/$branch/$buildnum/mac
    mkdir $tamarin/$branch/$buildnum/linux
    mkdir $tamarin/$branch/$buildnum/winmobile
    mkdir $tamarin/$branch/$buildnum/android
    chmod -R 777 $tamarin/$branch
}
