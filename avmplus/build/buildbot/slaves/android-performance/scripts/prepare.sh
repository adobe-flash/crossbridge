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


## Set the performance suite to use the !desktop datasizes, a.k.a. "mobile"
echo ""
echo  "Set the performance suite to use the !desktop datasizes, a.k.a. mobile"
cd $basedir/test/performance
mv dir.asc_args dir.asc_args.orig
sed "s/CONFIG::desktop=true/CONFIG::desktop=false/g" dir.asc_args.orig > dir.asc_args

# Move back into the default location
cd $basedir/build/buildbot/slaves/scripts
