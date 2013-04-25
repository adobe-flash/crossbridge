#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

# Do anything in this script that will help make sure the machine state is constant

##
# Bring in the environment variables
##
. ./environment.sh

## sync the private branch
../all/sync-private-branch.sh

exit 0
