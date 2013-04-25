#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


source=$1
test "$source" = "" && {
    echo "usage: util-upload-scp-mozilla.sh <source> <dest>"
    exit 1
}

dest=$2
test "$dest" = "" && {
    echo "usage: util-upload-scp-mozilla.sh <source> <dest>"
    exit 1
}


##
# Upload the requested file
##

## If we are running on cygwin then we need to make sure that the source
## path is a valid cygwin path.
uname=`uname `
if [[ $uname == CYGWIN* ]]; then
    source=`cygpath $source`
fi;

echo "Uploading $source to $dest"

## Just quickly make sure that the permisions are fully open on the file

chmod 744 $source
scp "$source" "$dest"
echo ""
echo ""

