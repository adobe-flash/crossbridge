#!/bin/bash
# -*-Mode: Shell-script; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

#export vmbuilds=ftp://ftp.mozilla.org/pub/js/tamarin/builds
export ftp_host=10.60.48.47
export vmbuilds=ftp://${ftp_host}/
#export ascbuilds=ftp://ftp.mozilla.org/pub/js/tamarin/builds/asc/latest
export ascbuilds=ftp://10.60.48.47/asc/latest/
export BUILTIN_BUILDFLAGS='-config CONFIG::VMCFG_FLOAT=false'

## Used by performance slaves for locking
export sleeptime=`expr $RANDOM % 60 + 60`

## Only set the branch if it has not already been set
echo branch [before]: $branch
if [ "$branch" == "" ]; then
    export branch=tamarin-redux
fi
export branch=`echo $branch | sed 's/-deep//' | sed 's/-performance//'`
echo branch [after]: $branch


workdir=`pwd`

# create the builds directory if it doesn't exist
if [ ! -d "${workdir}/../../../../../builds" ]; then
    mkdir -p ${workdir}/../../../../../builds
fi

if [ "$OSTYPE" == "cygwin" ]; then
    export basedir=`cygpath -m ${workdir}/../../../..`
    export buildsdir=`cygpath -m ${basedir}/../builds`
else
    export basedir=`cd ${workdir}/../../../..; pwd`
    export buildsdir=`cd ${basedir}/../builds; pwd`
fi

export ASC=$basedir/utils/asc.jar
export BUILTINABC=$basedir/generated/builtin.abc
export SHELLABC=$basedir/generated/shell_toplevel.abc

# builds for windows mobile (used by emulator)
export shell_release_arm=avmshell_arm$shell_extension


export shell=avmshell$shell_extension
export shell_selftest=avmshell$shell_extension
export shell_release=avmshell$shell_extension
export shell_release_wordcode=avmshell_wordcode$shell_extension
export shell_debug=avmshell_d$shell_extension
export shell_release_debugger=avmshell_s$shell_extension
export shell_debug_debugger=avmshell_sd$shell_extension

export shell_release_64=avmshell_64$shell_extension
export shell_release_wordcode_64=avmshell_wordcode_64$shell_extension
export shell_debug_64=avmshell_d_64$shell_extension
export shell_release_debugger_64=avmshell_s_64$shell_extension
export shell_debug_debugger_64=avmshell_sd_64$shell_extension
export shell_selftest_64=avmshell_64$shell_extension

export shell_release_min=avmshell_min$shell_extension
export shell_debug_min=avmshell_d_min$shell_extension

export shell_release_aot=avmshell_aot$shell_extension
export shell_debug_aot=avmshell_d_aot$shell_extension

export jit="-Ojit"
export interp="-Dinterp"


export ftp_asteam=10.60.48.47

export scp_mozilla="stage.mozilla.org:/home/ftp/pub/js/tamarin/builds"
export coverage_host=10.116.42.110
export coverage_dir="/cygdrive/e/AVMTeam/wwwroot/analytics/codecoverage"
export scp_coverage="${coverage_host}:${coverage_dir}"
export http_coverage=http://tamarin-builds.mozilla.org/analytics/codecoverage

export coverage_exclude_regions=`../all/codecoverage-excludes.py`

export doxygen_host=10.116.42.110
export doxygen_dir="/cygdrive/e/AVMTeam/wwwroot/doxygen"
export scp_doxygen="${doxygen_host}:${doxygen_dir}"

baselineBranchTime=tamarin-flash
baselineConfigTime=
baselineBuildTime=458340
baselineConfigTimeJIT=${mir}
baselineBuildTimeJIT=458340
baselineConfigTimeInterp=${interp}
baselineBuildTimeInterp=458340

baslineBranchMemory=${branch}
baselineConfigMemory=
baselineBuildMemory=966
baselineConfigMemoryJIT=${jit}
baselineBuildMemoryJIT=966
baselineConfigMemoryInterp=${interp}
baselineBuildMemoryInterp=966

configDefault=
configJIT=${jit}
configInterp=${interp}

# List of processes that should NEVER be running when the build is not
# currently running any tests. This list of process will be killed if the
# process is found. Process must not contain extension as cygwin will return
# the process without the extension. Used in all/util-process-clean.sh
export proc_names="avmshell ceremote"

# If available, use the python version specified in PYTHON_RUNTESTS instead of
# python that is in the path. There are a couple of reasons to do this:
# 1) Threading will not work with python 2.6 that is included with cygwin,
#    instead need to use python from python.org or ActiveState.
# 2) Python3 has better performance handling threads, but is not fully supported
#    in the build system, so only use to run acceptance suite.
if [ -z "$PYTHON_RUNTESTS" ]
then # Not set, just use python that is in the path
    export PYTHON_RUNTESTS=python
fi


# Helper functions to supress output
function beginSilent () {
    test "$silent" = "true" && {
        if [ ! -z internal_repo ]; then
            repo_url=`hg showconfig | grep paths.default | awk -F"=" '{print $2}'`
            if [[ "$repo_url" == http://hg.mozilla.org* ]]; then
                internal_repo=false
            else
                internal_repo=true
            fi
        fi
        test "$internal_repo" = "true" && {
            test "$logfile" = "" && {
                echo "logfile not defined, using generic name: output.log"
                logfile=output.log
            }
            # create log file in builds dir
            mkdir -p $buildsdir/${change}-${changeid}/$platform
            logfile=$buildsdir/${change}-${changeid}/$platform/$logfile
            touch $logfile
            exec 6>&1                # Link file descriptor #6 with stdout. (Saves stdout)
            exec >> $logfile 2>&1    # Redirect all stdout and stderr output to be appended to $logfile
        }
    }
}

function endSilent () {
    if [ "$silent" == "true" ] && [ "$internal_repo" == "true" ]; then
        exec 1>&6 2>&1 6>&-      # Restore stdout, stderr and close file descriptor #6.
        $workdir/../all/util-upload-ftp-asteam.sh $logfile $ftp_asteam/$branch/${change}-${changeid}/$platform/
        ret=$?
        if [ "$ret" != "0" ]; then
            echo "Uploading of $logfile failed"
            exit 1
        fi

        # ${logfile##*/} == filename from end of path
        echo "Build log can be found here: http://asteam.macromedia.com/builds/$branch/${change}-${changeid}/$platform/${logfile##*/}"
        echo "url: http://asteam.macromedia.com/builds/$branch/${change}-${changeid}/$platform/${logfile##*/} build log"
    fi
}

# Functions to download files to slave machine

##
# Download the latest asc.jar if it does not exist
##
function download_asc () {
    if [ ! -e "$ASC" ]; then
        echo "Download asc.jar"
        ../all/util-download.sh $ascbuilds/asc.jar $ASC
        ret=$?
        test "$ret" = "0" || {
            echo "Downloading of asc.jar failed"
            rm -f $ASC
            # call endSilent regardless of where this script is called from
            # as the endSilent function does the test of whether or not to execute
            endSilent
            exit 1
        }
    fi
}

##
# Download the AVMSHELL if it does not exist
# Requires a single argument: shell name to download
##
function download_shell () {
    if [ -z "$1" ]; then
        echo "ERROR: Missing Parameter.  Must pass the shell name as a parameter."
        return 1
    else
        local shell=$1
    fi
    
    if [ ! -e "$buildsdir/$change-${changeid}/$platform/$shell" ]; then
        echo "Download AVMSHELL: $shell"
        ../all/util-download.sh $vmbuilds/$branch/$change-${changeid}/$platform/$shell $buildsdir/$change-${changeid}/$platform/$shell
        ret=$?
        test "$ret" = "0" || {
            echo "Downloading of $shell failed"
            rm -f $buildsdir/$change-${changeid}/$platform/$shell
            # call endSilent regardless of where this script is called from
            # as the endSilent function does the test of whether or not to execute
            endSilent   
            exit 1
        }
        chmod +x $buildsdir/$change-${changeid}/$platform/$shell
    fi
}



