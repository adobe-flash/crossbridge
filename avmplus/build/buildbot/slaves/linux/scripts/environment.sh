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

export platform=linux

# required to force compiler to generate 32bit binaries
if [ -z "$CXX" ]; then
    export CXX="g++ -m32"
fi
if [ -z "$CC" ]; then
    export CC="gcc -m32"
fi

# List of processes that should NEVER be running when the build is not
# currently running any tests. This list of process will be killed if the
# process is found. Process must not contain extension as cygwin will return
# the process without the extension. Used in all/util-process-clean.sh
export proc_names="${shell_release}$ ${shell_debug}$ ${shell_release_debugger}$ ${shell_debug_debugger}$"