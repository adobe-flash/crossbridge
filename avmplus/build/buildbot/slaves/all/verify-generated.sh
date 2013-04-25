#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

echo basedir=$basedir

## This function is called by verify_builtinabc.sh and verify_tracers.sh
function verify_generated_files () {

    export LC_ALL=C
    
    ##
    # Backup the generated files
    ##
    cd $basedir/generated
    for file in $FILES
    do
        # The python scripts that build the files are expecting a specific list of files so fail here if one is missing.    
        if [ ! -e "$file" ]; then       
            echo "Expected file $file does not exist."; echo
            endSilent
            export LC_ALL=
            exit 1
        else
            mv $file $file".orig"
        fi
    done

    ##
    # Build the files
    ##
    echo "building $FILES"
    cd $basedir/"$BUILDER_DIR"
    python ./$BUILDER
    ret=$?
    test "$ret" = "0" || {
        echo "$BUILDER failed"
        cd $basedir/generated
        for file in $FILES
        do
            if [ ! -e "$file" ]; then
                echo "Expected file $file.orig does not exist."; echo
            else
                mv $file".orig" $file
            fi
        done
        endSilent
        export LC_ALL=
        exit 1        
    }
    
    ##
    # Diff the files
    ##
    cd $basedir
    for file in $FILES
    do    
        test -f $basedir/build/buildbot/slaves/scripts/$file".diff" && {
            rm $basedir/build/buildbot/slaves/scripts/$file".diff"
        }
        
        tr -d '\r' < generated/$file > generated/difftemp
        tr -d '\r' < generated/$file.orig > generated/difftemp.orig
        
        diff generated/difftemp generated/difftemp.orig
        diff generated/difftemp generated/difftemp.orig > $basedir/build/buildbot/slaves/scripts/$file.diff
        
        rm generated/difftemp
        rm generated/difftemp.orig
        
        # If the diff file is > 0 bytes fire a warning
        if [ -s $basedir/build/buildbot/slaves/scripts/$file.diff ]; then
            echo "buildbot_status: WARNINGS"
        fi
    done
    
    # Revert files
    # The files are being touched below to deal with this issue on solaris:
    # http://hg.mozilla.org/tamarin-redux/rev/67a99bdafb0c
    cd $basedir/generated
    for file in $FILES
    do
        mv $file".orig" $file
        # don't touch abc files
        if [ ${file/*./} != "abc" ]; then
            touch $file
        fi
    done
    
    export LC_ALL=
}

