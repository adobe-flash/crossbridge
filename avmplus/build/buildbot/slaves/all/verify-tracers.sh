#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1

# silence output if silent=true (function defined in environment.sh)
logfile=verify_tracers_${shell_release}.log
beginSilent

##
# Download the latest asc.jar if it does not exist
##
download_asc

# exactgc.py, called by builtin-tracers.py and shell_toplevel-tracers.py below, builds against the avmshell
# so check that we're not cross-compiling and the shell will run on the build platform.
# Set AVM
    if [ "$AVM" == "" ]; then
        export AVM=$buildsdir/$change-${changeid}/$platform/$shell_release
    fi
echo "Cross-compile check: Running avmshell at $AVM"    
$AVM | grep ${change}:${changeid} &> /dev/null
res=$?
if [ "$res" != "0" ]; then
    echo ""
    echo "Cross-compile check failed - cannot build exactgc tracers on $platform platform"
    echo ""
    endSilent
    exit 0
else
    # The verify-generated.sh script has a common function called by this script and verify_builtinabc.sh
    . ../all//verify-generated.sh

    echo ""
    echo "Building exactgc tracer files"
    echo ""
    
    FILES="avmplus-tracers.h avmplus-tracers.hh"
    BUILDER="builtin-tracers.py"
    BUILDER_DIR="core"
    verify_generated_files $FILES $BUILDER $BUILDER_DIR
    
    FILES="avmshell-tracers.h avmshell-tracers.hh extensions-tracers.h extensions-tracers.hh"
    BUILDER="shell_toplevel-tracers.py"
    BUILDER_DIR="shell"
    verify_generated_files $FILES $BUILDER $BUILDER_DIR
fi

endSilent

exit 0
