#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


source=$1
test "$source" = "" && {
    echo "usage: util-upload-ftp-asteam.sh <source> <dest>"
    exit 1
}

dest=$2
test "$dest" = "" && {
    echo "usage: util-upload-ftp-asteam.sh <source> <dest>"
    exit 1
}


##
# Upload the requested file
##

echo "Uploading $source to $dest"

## Just quickly make sure that the permisions are fully open on the file
chmod 777 $source

maxtries=5
x=1

while [ $x -le $maxtries ]
do
    curl --fail --ftp-create-dirs -T $source ftp://${dest}
    res=$?
    if [ "$res" != "0" ]; then
	echo "Upload appears to have failed, exitcode: $res"
        if [ $x -lt $maxtries ]; then
            x=$(( $x + 1 ))
            echo "Attempting upload again.  Attempt $x of $maxtries."
            sleep 5
        else 
            echo "Upload failed after $maxtries tries.  Aborting upload."
            exit 1
        fi
    else
	echo "Upload successful"
	exit 0
    fi
done

exit 0
