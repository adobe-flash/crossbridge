#!/bin/bash
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


# This script takes the given revision and promotes it to the "latest" directory
# on asteam and also copies the files to the mozilla ftp site

# script is designed to only run on asteam

# $change contains the revision id we want to promote, $changeid is the hash
pushd $buildsdir

# verify the build exists
if [[ -d "$change-$changeid" ]]; then
    echo "$change-$changeid" >> greenBuilds.txt
    rm -r latest
    cp -R $change-$changeid latest
    echo "$change" > latest/revision.txt
    
    # promote the build on mozilla server
    # setup directory and delete older revs
    ssh stage.mozilla.org "~/setupbuilds.sh $branch $change-$changeid"
    scp -r $change-$changeid $scp_mozilla/$branch/
else
    echo "Revision to promote: $change-$changeid NOT FOUND in builds/tamarin-redux"
    exit 1
fi



