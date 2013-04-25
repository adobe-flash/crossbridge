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

export platform=android

export shell_release=avmshell$shell_extension
export shell_release_arm=$shell_release

export slavename="$branch-$platform-performance"

export hostip=10.60.147.120

baselineBranchTime=tamarin-argo
baselineConfigTime=
baselineBuildTime=3765
baselineConfigTimeJIT=${jit}
baselineBuildTimeJIT=3765
baselineConfigTimeInterp=${interp}
baselineBuildTimeInterp=3765

# Override this, default is avmshell* and since this slave runs on a machine
# with other slaves, the process cleaner /can/ find avmshell processes,
# BUT they will NEVER belong to this slave since the shell is run on a device
# not on the host machine. Reset this to something that will never be found/killed
export proc_names="fake_never_find_me"
