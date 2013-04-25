#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1


mkdir -p $buildsdir/$change-${changeid}/

archive_file=$buildsdir/$change-${changeid}/tamarin-private.zip

test -f $archive_file || {
  wget --output-document=$archive_file http://10.60.48.47/hg/tamarin-redux-private/archive/tip.zip
}

## Clean up the old zip contents if it exists
cd $buildsdir/$change-${changeid}/
rm -rf tamarin-redux-private*

## The files will unzip into a tamarin-redux-private-HASH directory
## so just cd into tamarin-redux-private* since we don't know the hash
unzip -q $archive_file
cd tamarin-redux-private*

## Remove the .hg info file so that it is not copied into the main repo
rm .hg_archival.txt

## Copy the private repo onto the public repo
echo "===================="
echo ""
echo "Applying additional files to the repository"
echo ""
cp -Rv ./ $basedir
echo ""

## Return back to the scripts direcotry
cd $basedir/build/buildbot/slaves/scripts
