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

export CXX='mipsel-linux-uclibc-g++ -static'
export CC='mipsel-linux-uclibc-gcc -static'
export AR=mipsel-linux-uclibc-ar
export LD=mipsel-linux-uclibc-ld

export shell_release=avmshell_mips
export shell_debug=avmshell_mips_d
export ssh_proc_names="avmshell_mips avmshell_mips_d avmshell_mips_osr avmshell_mips_aot"

# Override the default MAKE_OPTIONS env variable on the machine
export MAKE_OPTIONS="-j2"

# Override this, default is avmshell* and since this slave runs on a machine
# with other slaves, the process cleaner /can/ find avmshell processes, 
# BUT they will NEVER belong to this slave since the shell is run on a device
# not on the host machine. Reset this to something that will never be found/killed
export proc_names="fake_never_find_me"

export PYTHON_RUNTESTS=python3

export threads=4

export SSH_SHELL_REMOTE_HOST0=asteammips2
export SSH_SHELL_REMOTE_USER0=root
export SSH_SHELL_REMOTE_BASEDIR0=/root
export SSH_SHELL_REMOTE_DIR0=/root/app1

export SSH_SHELL_REMOTE_HOST1=asteammips2
export SSH_SHELL_REMOTE_USER1=root
export SSH_SHELL_REMOTE_BASEDIR1=/root
export SSH_SHELL_REMOTE_DIR1=/root/app2

export SSH_SHELL_REMOTE_HOST2=asteammips4
export SSH_SHELL_REMOTE_USER2=root
export SSH_SHELL_REMOTE_BASEDIR2=/root
export SSH_SHELL_REMOTE_DIR2=/root/app1

export SSH_SHELL_REMOTE_HOST3=asteammips4
export SSH_SHELL_REMOTE_USER3=root
export SSH_SHELL_REMOTE_BASEDIR3=/root
export SSH_SHELL_REMOTE_DIR3=/root/app2
