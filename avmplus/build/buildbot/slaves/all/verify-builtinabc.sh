#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

# silence output if silent=true (function defined in environment.sh)
logfile=verify_builtinabc_${shell_release}.log
beginSilent

##
# Download the latest asc.jar if it does not exist
##
download_asc

# The verify-generated.sh script has a common function called by this script and verify_tracers.sh
. ../all/verify-generated.sh

echo ""
echo "Building builtin.abc and shell_toplevel.abc using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

FILES="builtin.abc builtin.cpp builtin.h"
BUILDER="builtin.py $BUILTIN_BUILDFLAGS"
BUILDER_DIR="core"
verify_generated_files $FILES $BUILDER $BUILDER_DIR

FILES="shell_toplevel.abc shell_toplevel.cpp shell_toplevel.h"
BUILDER="shell_toplevel.py $BUILTIN_BUILDFLAGS"
BUILDER_DIR="shell"
verify_generated_files $FILES $BUILDER $BUILDER_DIR

endSilent

exit 0

