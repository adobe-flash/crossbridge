#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

cwd=`pwd`
cd $basedir
if [ -d .hg ];
then
    withhash=`echo $1 | grep :`
    # nothing passed in
    if [ "$1" == "" ]; then
        change=`hg identify -n | awk -F+ '{print $1}'`
        echo "change number not passed using tip: $change"
        changeid=`hg log -r $change | head -n 1 | awk -F: '{print $3}'`

    # Check to see if just a hash was passed in, do this naively by checking length
    elif [ "${#1}" == 12  ]; then
        changeid=$1
        change=`hg log -r $changeid | head -n 1 | awk '{print $2}'|awk -F: '{print $1}'`
        echo "changeid hash passed in: $changeid"

    # only revision passed in
    elif [ "$withhash" == ""  ]; then
        if [ "$1" == "tip" ]; then
            change=`hg identify -n | awk -F+ '{print $1}'`
        else
            change=$1
        fi
        echo "change number passed: $change"
        changeid=`hg log -r $change | head -n 1 | awk -F: '{print $3}'`

    # revision and changeid passed in
    else
        change=${1%:*}
        changeid=${1#*:}
        echo "change and changeid number passed: $change $changeid"
    fi
    
    echo "running $platform build for change: $change changeid: $changeid"


else
    change=$1
    test "$change" = "" && {
        change=`p4 changes -m 1 //depot/main/FlashRuntime/Main/code/third_party/avmplus/... | grep Change | awk '{print $2}'`
        echo "change number no passed using tip: $change"
    }
    change=`echo $change | awk -F: '{print $1}'`
    changeid=

fi

# Move back into the default location
cd $cwd
