#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh



echo
echo "+++++++++++++++++++++++++++++++++++++++++++++"
echo "Ensure that the system is torn down and clean"
echo "+++++++++++++++++++++++++++++++++++++++++++++"
echo
##
# Make sure that there are no processes running that should not be
##
../all/util-process-clean.sh

echo; echo;
