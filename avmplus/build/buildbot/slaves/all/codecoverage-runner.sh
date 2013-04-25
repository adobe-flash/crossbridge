#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

# This is just a wrapper script to the python script, it is used in order
# to get the environment variables set via the environment.sh script.

##
# Bring in the environment variables
##
. ./environment.sh

python ../all/codecoverage-runner.py $*

