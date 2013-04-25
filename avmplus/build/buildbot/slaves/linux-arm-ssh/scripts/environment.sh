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

export CXX=arm-none-linux-gnueabi-g++
export CC=arm-none-linux-gnueabi-gcc
export AR=arm-none-linux-gnueabi-ar
export LD=arm-none-linux-gnueabi-ld

export shell_release=avmshell_neon_arm
export shell_release_debugger=avmshell_neon_arm
export shell_debug=avmshell_neon_arm_d
export shell_debug_debugger=avmshell_neon_arm_d
export shell_selftest=avmshell_neon_arm

export ssh_proc_names="avmshell_neon_arm avmshell_neon_arm_d"

# Override this, default is avmshell* and since this slave runs on a machine
# with other slaves, the process cleaner /can/ find avmshell processes, 
# BUT they will NEVER belong to this slave since the shell is run on a device
# not on the host machine. Reset this to something that will never be found/killed
export proc_names="fake_never_find_me"

#export PYTHON_RUNTESTS=python3
export threads=2

export SSH_SHELL_REMOTE_HOST0=asteambeagle5
export SSH_SHELL_REMOTE_USER0=build
export SSH_SHELL_REMOTE_BASEDIR0=/home/build
export SSH_SHELL_REMOTE_DIR0=/home/build/app1

export SSH_SHELL_REMOTE_HOST1=asteambeagle5
export SSH_SHELL_REMOTE_USER1=build
export SSH_SHELL_REMOTE_BASEDIR1=/home/build
export SSH_SHELL_REMOTE_DIR1=/home/build/app2
