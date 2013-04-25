#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Set any variables that my be needed higher up the chain
##
export shell_extension=

##
# Bring in the BRANCH environment variables
##
. ../all/environment.sh

export platform=mac

export shell_release=${shell_release}_64

lockfile=~/buildbot.lck
export slavename="$branch-$platform-performance"


export hostip=10.60.144.182

PATH=/usr/local/bin/:$PATH

baselineBranchTime=tamarin-central
baselineConfigTime=
baselineBuildTime=707
baselineConfigTimeJIT=${jit}
baselineBuildTimeJIT=707
baselineConfigTimeInterp=${interp}
baselineBuildTimeInterp=707
