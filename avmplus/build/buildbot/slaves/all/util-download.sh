#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


source=$1
test "$source" = "" && {
    echo "usage: util-download.sh <source> <dest>"
    exit 1
}

dest=$2
test "$dest" = "" && {
    echo "usage: util-download.sh <source> <dest>"
    exit 1
}

##
# Make sure the destination directory exists
##
dest_dir=`dirname $dest`
mkdir -p $dest_dir

##
# Download the requested file
##
echo "Downloading $source to $dest"

maxtries=5
x=1

while [ $x -le $maxtries ]
do
    
    test -f download.log && {
       rm -f download.log
    }
    
    # wget "-O -" tells wget to print download to stdout.  This is then piped to our
    # $dest file.  The reason for using this and not doing a "-O $dest" is if there
    # is an error midstream, wget will retry the download and APPEND to $dest (which
    # already is a partially downloaded file) resulting in a corrupt download
    
    wget -o download.log --progress=dot:mega -O - $source > $dest
    cat download.log
    response=`cat download.log | grep ERROR`
    if [ ! -z "${response}" ]; then
            echo "Did not find $source, exit run"
            exit 1
    fi
    
    ##
    # Check that the source file is the same size that wget indicated
    ##
    
    # use nawk if present (due to solaris awk not being compatible)
    AWK=awk
    test `which nawk` && {
        AWK=nawk
    }
    
    # awk command breakdown - need to account for variation in wget output on diff platforms:
    # /(saved|written to stdout).*\[/ = find the line with "saved [" or "written to stdout ["
    # match(...) = using the line above regexp match a "[" followed by digits followed by either "]" or "/"
    # match defines RSTART = index of where the pattern starts; RLENGTH = length of pattern match
    # if ... print = if there's a match, print the pattern minus the surrounding chars
    expectedFilesize=`$AWK  '/(saved|written to stdout) \[/ {match($0,/\[([0-9]+)(\]|\/)/); if (RSTART) print substr($0, RSTART+1, RLENGTH-2)}' download.log`
    if [ "$expectedFilesize" = "" ]; then
        expectedFilesize=-1
    fi

    if [ `uname -o` = "Cygwin" ]
    then
        # on windows, the ls columns are not quoted
        # and the "group" may contain a space;
        # use "du -sb" to return size in bytes
        filesize=`du -sb $dest | awk '{print $1}'`
    else
        # osx du command doesn't have a bytes option
        filesize=`ls -l $dest | awk '{print $5}'`
    fi
    
    echo "Downloaded file is $filesize bytes"
    if [ "$filesize" -eq "$expectedFilesize" ]; then
        echo "Download successful"
        exit 0
    else
        echo "Download appears to have failed"
        rm $dest
        if [ $x -lt $maxtries ]; then
            x=$(( $x + 1 ))
            echo "Attempting download again.  Attempt $x of $maxtries."
            sleep 5
        else 
            echo "Download failed after $maxtries tries.  Aborting Download."
            exit 1
        fi
    fi
done

exit 0
