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

export shell_release_cov=avmshell_cov.exe
export shell_release_debugger_cov=avmshell_s_cov.exe
export shell_debug_cov=avmshell_d_cov.exe
export shell_debug_debugger_cov=avmshell_sd_cov.exe

export shell_release_vtune=avmshell_vtune.exe

export build_shell_release_cov=avmshell_cov
export build_shell_release_debugger_cov=avmshell_s_cov
export build_shell_debug_cov=avmshell_d_cov
export build_shell_debug_debugger_cov=avmshell_sd_cov

export bullseyedir="/c/Progra~1/BullseyeCoverage/bin"


VS_HOME_PATH="/c/Program Files/Microsoft Visual Studio 9.0"
VS_HOME="c:\Program Files\Microsoft Visual Studio 9.0"

# ${PREPATH} if it exists will be the first thing in the path,
# this is needed for code coverage.
export PATH="${PREPATH}:$VS_HOME_PATH/Common7/IDE:$VS_HOME_PATH/VC/bin:$VS_HOME_PATH\Common7\Tools:$VS_HOME_PATH/VC/VCPackages:$PATH"
export INCLUDE="c:\Program Files\Microsoft SDKs\Windows\v6.0\Include;$VS_HOME\VC\atlmfc\include;$VS_HOME\VC\include;C:\Program Files\Intel\VTune\Analyzer\Include"
export LIB="c:\Program Files\Microsoft SDKs\Windows\v6.0\Lib;$VS_HOME\VC\atlmfc\lib;$VS_HOME\VC\lib;C:\Program Files\Intel\VTune\Analyzer\Lib;"
export LIBPATH="c:\Program Files\Microsoft SDKs\Windows\v6.0\Lib;$VS_HOME\VC\atlmfc\lib;$VS_HOME\VC\lib;C:\Program Files\Intel\VTune\Analyzer\Lib;"

