#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

test "$lockfile" = "" && {
  echo "must set lockfile env variable"
  exit 1
}
test "$sleeptime" = "" && {
  echo "must set sleeptime env variable"
  exit 1
}
echo "Attempting to acquire lock."
while true
do
    echo Will check for lock in $sleeptime seconds.
    sleep $sleeptime

    # Check to see if we actually own the lock and it is stale
    test -f $lockfile && {
        lock_owner=`head -1 $lockfile | cut -d: -f1`
        test $lock_owner == $slavename && {
            echo There is a stale lockfile, removing stale lock.
            rm -f $lockfile
        }
    }
    test -f $lockfile || {
        echo Lock has been acquired
        echo $slavename: `date` > $lockfile
        break;
    }

    echo [`date +%H:%M:%S`] Lock in use by: `cat $lockfile`
done

exit 0

