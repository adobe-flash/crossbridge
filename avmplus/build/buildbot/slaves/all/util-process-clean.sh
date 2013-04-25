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
# Make sure that there are no processes running that should not be
##

for process in ${proc_names}
do
    prevPID=-0
    while [ 1 ]
    do
        if [[ `uname` == *-WOW64* ]]; then
            # If running on win64 make sure to install plist from 
            # http://technet.microsoft.com/en-us/sysinternals/bb896682.aspx
            PID=`pslist ${process} | tail -1 | awk '{print $2}'`
        elif [[ `uname` == *CYGWIN* ]]; then
            PID=`ps -aW | grep "${process}" | grep -v grep | grep -v ps |\
                 tail -1 | awk '{print $1}'`
        else
            PID=`ps -Ac | grep "${process}" | grep -v grep | grep -v ps |\
                 tail -1 | awk '{print $1}'`
        fi

        if [ "$prevPID" == "$PID" ]; then
            echo 
            echo "Unable to kill zombie ${process} process with pid $PID"
            echo "buildbot_status: WARNINGS"
            echo
            break
        fi
        if [ "" != "$PID" ]; then
            echo
            echo "There is a zombie ${process} [pid $PID] still running."
            echo "Attempting to kill process $PID"
            echo "message: zombie ${process} process found"
            echo "buildbot_status: WARNINGS"
            echo

            if [[ `uname` == *-WOW64* ]]; then
                echo "`pslist $PID`"
                # bash: kill: (####) - No such process" error using kill 
                # command in Cygwin
                # http://www-01.ibm.com/support/docview.wss?uid=swg21205470
                /bin/kill -f $PID
            elif [[ `uname` == *CYGWIN* ]]; then
                echo "`ps -W -p $PID`"
                # bash: kill: (####) - No such process" error using kill 
                # command in Cygwin
                # http://www-01.ibm.com/support/docview.wss?uid=swg21205470
                /bin/kill -f $PID
            else
                echo "`ps -p $PID`"
                kill -9 $PID
            fi
            
            prevPID=$PID
            sleep 10
        else
            break
        fi
    done
done

