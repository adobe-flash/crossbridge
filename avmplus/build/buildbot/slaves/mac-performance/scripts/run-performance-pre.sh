#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh


prevPID=-0
while [ 1 ]
do
    # Need to grep for "avmshell" instead of $shell_release since cygwin will return 
    # the process without the extension, which is part of $shell_release.
    avmshellPID=`ps -c | grep avmshell | grep -v grep | grep -v ps | tail -1 | awk '{print $1}'`
    if [ "$prevPID" == "$avmshellPID" ]; then
        echo ""
        echo "Unable to kill running $shell_release process with pid $avmshellPID"
        echo "buildbot_status: WARNINGS"
        break
    fi
    if [ "" != "$avmshellPID" ]; then
        echo ""
        echo "There is a rogue $shell_release process [pid $avmshellPID] still running."
        echo "Attempting to kill process $avmshellPID"
        echo "message: results are NOT reliable"
        echo "buildbot_status: WARNINGS"
        kill -9 $avmshellPID
        prevPID=$avmshellPID
        sleep 10
    else
        break
    fi
done

