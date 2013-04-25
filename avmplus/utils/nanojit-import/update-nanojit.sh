#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

#
# This script automates the import of changes from
# nanojit-central to tamarin. 
#
# It works by noting revisions from nanojit-central, starting with the
# rev number in the file nanojit-import-rev, and then pulling those
# changes into the current repo.
#
# The last step updates the rev number file and checks it in.
#
# Note that the scripts will clobber the contents of the nanojit/
# directory so it is important that changes move in a single
# direction; i.e. from nanojit-central --> tamarin.
#
# You also need to ensure that nothing nasty happened during this
# import, and that you're the only person using the script when you
# push your changes.
#
# I would also recommend only running the script once per 'checkin',
# and not try to revert or otherwise manipulate the repo after the
# script has run.
#
# If nanojit-import-rev (which identifies a node in nanojit-central)
# gets corrupted (it really should never happen) you need to manually
# set it to the correct rev, not doing so will cause subsequent
# executions of this script to do very nasty things.
#
# I found the following command quite handy in displaying
# rev information: hg log --template "{rev} {node|short} {node}
# {author}\n", since the long form of the node is used in the file. 
# 
# If you see the error "mercurial.error.RepoError: unknown revision xxx", then
# the NJ repo you're attempting to pull from does not have the rev identified
# in the file nanojit-import-rev
# 
set -x
set -e

if [ $# -lt 1 ]
then
    echo "usage: $0 rev"
    echo "Pulls and commits the latest nanojit-central changes to this repo"
    echo "rev can be tip or a specific rev to pull"
    exit 1
fi

# random part of names
RAND="$RANDOM$RANDOM"

SRCDIR=$(hg root)
SCRIPTDIR=$SRCDIR/utils/nanojit-import
TMPDIR=/tmp/nj-$RAND

NANOJIT_CENTRAL_NEW_REV=$1
NANOJIT_CENTRAL_REV=$(cat $SCRIPTDIR/nanojit-import-rev)
NANOJIT_CENTRAL_REPO=http://hg.mozilla.org/projects/nanojit-central
NANOJIT_CENTRAL_LOCAL=$TMPDIR/nanojit-central

if [ $# -eq 2 ]
then
	# undocumented option ... be mighty careful when using could result in big problems
	NANOJIT_CENTRAL_REPO=$2
fi

mkdir $TMPDIR
rm -Rf $NANOJIT_CENTRAL_LOCAL $TMPDIR/import-splicemap $TMPDIR/import-revmap

hg clone -r$NANOJIT_CENTRAL_NEW_REV $NANOJIT_CENTRAL_REPO $NANOJIT_CENTRAL_LOCAL

python $SCRIPTDIR/find-child.py \
    --src=$NANOJIT_CENTRAL_LOCAL \
    --dst=$SRCDIR \
    --start=$NANOJIT_CENTRAL_REV \
    --filemap=$SCRIPTDIR/nanojit-import-filemap \
    >$TMPDIR/import-splicemap \
|| exit 0

hg convert --config convert.hg.saverev=True \
    --config convert.hg.startrev=$(cut -d ' ' -f 1 $TMPDIR/import-splicemap) \
    --filemap=$SCRIPTDIR/nanojit-import-filemap \
    --splicemap=$TMPDIR/import-splicemap \
    $NANOJIT_CENTRAL_LOCAL \
    $SRCDIR \
    $TMPDIR/import-revmap

(cd $SRCDIR && \
    hg up)

(cd $NANOJIT_CENTRAL_LOCAL && \
    hg log -r tip --template "{node}\n") >$SCRIPTDIR/nanojit-import-rev

(cd $SRCDIR && \
    hg commit --message="Update nanojit-import-rev stamp." $SCRIPTDIR/nanojit-import-rev)

set +x
echo "This repo is now ready to push changes...ensure there is only a single head first"
echo "You may also want to clean-up the tmp directory at " $TMPDIR
