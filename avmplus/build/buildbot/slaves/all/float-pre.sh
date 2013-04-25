#!/bin/bash
# -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh

# Override some variables to enable float
export ascbuilds=http://asteam.corp.adobe.com/hg/asc-float/raw-file/tip/lib/
export BUILTIN_BUILDFLAGS='-config CONFIG::VMCFG_FLOAT=true'

##
# Download the float enabled asc.jar
##
rm $basedir/utils/asc.jar
download_asc

echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

echo ""
echo "Rebuild all of the builtins with float enabled"
echo ""
cd $basedir/core
python builtin.py $BUILTIN_BUILDFLAGS

cd $basedir/shell
python shell_toplevel.py
