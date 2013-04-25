#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

test -f $lockfile || {
    echo Lock does not exist.
    exit 0
}
lockname=`head -1 $lockfile | cut -d: -f1`


echo "Releasing the lock."

if test "${lockname}" = "${slavename}"; then
    rm -f $lockfile
else
    echo The lock file is not owned by this slave, will not remove the lock.
fi

exit 0
