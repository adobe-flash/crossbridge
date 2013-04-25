#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

echo "Deleting all folders more than 7 days old under: $buildsdir"

# -p points to the top-level /builds dir; -i is how many days to preserve build folders.
python ../all/util-clean-buildsdir.py -p $buildsdir -i 7
ret=$?
test "$ret" = "0" || {
    echo "util-clean-buildsdir.py failed"
}
exit 0
