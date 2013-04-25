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

# Override this, default is avmshell* and since this slave runs on a machine
# with other slaves, the process cleaner /can/ find avmshell processes, 
# BUT they will NEVER belong to this slave since the shell is run on a device
# not on the host machine. Reset this to something that will never be found/killed
export proc_names="fake_never_find_me"

