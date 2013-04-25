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

##
# Download the normal asc.jar
##
rm $basedir/utils/asc.jar
download_asc

echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

echo ""
echo "Revert the builtins to be the normal/non-float versions"
echo ""
cd $basedir

# check to see if we're running under Jenkins
if [ "$JENKINS_HOME" != "" ]; then
    p4 sync -f $WS/generated/...
else
    hg revert generated/
fi
