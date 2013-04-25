#!/bin/bash
# -*-Mode: Shell-script; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh

##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1

# Need to export the change and changeid so that doxygen can see the values
export change=${change}
export changeid=${changeid}

##
# Make sure that docs directory is clean
##
cd $basedir
test -d docs && {
    echo Remove directory $basedir/docs
    rm -rf $basedir/docs
}
mkdir $basedir/docs

# build the doxygen docs
doxygen Doxyfile
res=$?
test "$res" = "0" || {
    echo "doxygen failed, return value $res"
    exit 1
}

# zip up the source to move to tamarin-builds
cd $basedir/docs
zip -r doxygen.zip html/

echo; echo; echo "===> Copy the new doxygen docs to tamarin-builds"
scp doxygen.zip ${scp_doxygen}

echo; echo; echo "===> Remove the old doxygen docs from tamarin-builds"
ssh ${doxygen_host} "cd ${doxygen_dir}; rm -rf html/"

echo; echo; echo "===> Unpack the doxygen.zip"
ssh ${doxygen_host} "cd ${doxygen_dir}; unzip doxygen.zip"

echo; echo; echo "===> Clean up the doxygen.zip file"
ssh ${doxygen_host} "cd ${doxygen_dir}; rm doxygen.zip"

echo; echo; echo "===> Clean up the local repo"
cd $basedir
rm -rf docs/

exit 0
