#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

export shell_release=$shell_release_64
export shell_release_wordcode=$shell_release_wordcode_64
export shell_debug=$shell_debug_64
export shell_release_debugger=$shell_release_debugger_64
export shell_debug_debugger=$shell_debug_debugger_64
export shell_selftest=$shell_selftest_64


if [ "$VS_HOME" == "" ]
then
    VS_HOME="C:\Program Files (x86)\Microsoft Visual Studio 9.0"
fi
VS_HOME_PATH=`cygpath -a "$VS_HOME"`
echo VS_HOME: $VS_HOME
echo VS_HOME_PATH: $VS_HOME_PATH

if [ "$WINDOWS_SDK" == "" ]
then
    WINDOWS_SDK="c:\Program Files\Microsoft SDKs\Windows\v6.0A"
fi

export PATH="$PATH_PREPEND:$VS_HOME_PATH/Common7/IDE:$VS_HOME_PATH/VC/bin/amd64:$VS_HOME_PATH\Common7\Tools:$VS_HOME_PATH/VC/VCPackages:$PATH"
export INCLUDE="$WINDOWS_SDK\Include;$VS_HOME\VC\atlmfc\include;$VS_HOME\VC\include;"
export LIB="$WINDOWS_SDK\Lib\x64;$VS_HOME\VC\atlmfc\lib\amd64;$VS_HOME\VC\lib\amd64;"
export LIBPATH="$WINDOWS_SDK\Lib\x64;$VS_HOME\VC\atlmfc\lib\amd64;$VS_HOME\VC\lib\amd64;"
