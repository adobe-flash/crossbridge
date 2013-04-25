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

export shell_release=${shell_release}_64
export shell_release_wordcode=${shell_release_wordcode}_64
export shell_debug=${shell_debug}_64
export shell_release_debugger=${shell_release_debugger}_64
export shell_debug_debugger=${shell_debug_debugger}_64
export shell_selftest=${shell_selftest}_64

export shell_release_cov=avmshell_cov
export shell_release_debugger_cov=avmshell_s_cov
export shell_debug_cov=avmshell_d_cov
export shell_debug_debugger_cov=avmshell_sd_cov

export build_shell_release_cov=avmshell_cov
export build_shell_release_debugger_cov=avmshell_s_cov
export build_shell_debug_cov=avmshell_d_cov
export build_shell_debug_debugger_cov=avmshell_sd_cov

export bullseyedir=/home/build/tools/bullseye/bin

