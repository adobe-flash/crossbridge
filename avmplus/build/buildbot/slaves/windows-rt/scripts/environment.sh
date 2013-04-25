#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Set any variables that my be needed higher up the chain
##
export shell_extension=.exe

##
# Bring in the BRANCH environment variables
##
. ../all/environment.sh

export platform=windows

export shell_release=$shell_release_rt
export shell_debug=$shell_debug_rt
export shell_release_debugger=$shell_release_debugger_rt
export shell_debug_debugger=$shell_debug_debugger_rt


if [ "$VS_HOME" == "" ]
then
    VS_HOME="C:\Program Files (x86)\Microsoft Visual Studio 11.0"
fi
VS_HOME_PATH=`cygpath -a "$VS_HOME"`
echo VS_HOME: $VS_HOME
echo VS_HOME_PATH: $VS_HOME_PATH

if [ "$WINDOWS_SDK" == "" ]
then
    WINDOWS_SDK="C:\Program Files (x86)\Windows Kits\8.0"
fi

export PATH="$PATH_PREPEND:$VS_HOME_PATH/Common7/IDE:$VS_HOME_PATH/VC/bin/x86_arm:$VS_HOME_PATH\Common7\Tools:$VS_HOME_PATH/VC/VCPackages:$PATH"

export INCLUDE="$VS_HOME\VC\atlmfc\include;$VS_HOME\VC\include;$WINDOWS_SDK\Include\um;$WINDOWS_SDK\Include\shared;"

export LIB="$VS_HOME\VC\atlmfc\lib\arm;$VS_HOME\VC\lib\arm;$WINDOWS_SDK\Lib\win8\um\arm"
export LIBPATH="$LIB;"
