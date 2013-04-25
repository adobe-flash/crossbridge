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

showhelp ()
{
    echo ""
    echo "usage: compile-generic.sh <change> <config> <filename> <upload>"
    echo "       <change>       changeset that is going to be built"
    echo "       <config>       config options passed to configure.py"
    echo "       <filename>     name of the shell, do not include file extension"
    echo "       <upload>       (true|false) upload shell to server"
    echo "       <features>     +<feature> -<feature>, will ensure specified"
    echo "                      features are either enabled(+) or disabled(-)"
    echo "       <compiledir>   directory to build in (default=objdir)"
    exit 1
}

config=$2
test "$config" = "" && {
    showhelp
}
test "$CONFIGURE_ARGS" != "" && {
    echo "CONFIGURE_ARGS=$CONFIGURE_ARGS, appending to config"
    config="$config $CONFIGURE_ARGS"
}

filename=$3
test "$filename" = "" && {
    showhelp
}

upload=$4
test "$upload" = "true" || {
    upload=false
}

# Features to confirm being enabled or disabled
features=$5

compiledir=$6
test "$compiledir" = "" && {
    compiledir=objdir
}


# silence output if silent=true (function defined in environment.sh)
logfile=build-$platform-$filename.log
beginSilent

##
# Update the version string
##

. ../all/util-update-version.sh

##
# Download the latest asc.jar if it does not exist
##
download_asc

##
# Make sure that there are no left over directories from previous compile
##
cd $basedir
test -d ${compiledir} && {
    echo Remove directory $basedir/${compiledir}
    rm -rf $basedir/${compiledir}
}

mkdir ${compiledir}

cd ${compiledir}

echo ""
echo "*******************************************************************************"
echo "configure.py call:"
echo "../configure.py $config"
python ../configure.py $config
echo ""
echo "*******************************************************************************"

echo ""
echo "*******************************************************************************"
echo "Makefile:"
echo ""
cat Makefile
echo ""
echo "*******************************************************************************"
topsrcdir=`grep topsrcdir= Makefile | awk -F"=" '{print $2}'`
CXX=`grep CXX= Makefile | awk -F"=" '{print $2}'| sed 's/(/{/' | sed 's/)/}/' | sed 's/-nologo//'`
echo compiler version:
if [ `uname` == "SunOS" ]; then
    eval ${CXX} -V
else
    eval ${CXX} --version
fi
echo ""
echo "*******************************************************************************"
echo ""


make $MAKE_OPTIONS clean

echo ""
echo "*******************************************************************************"
echo ""
echo "make $MAKE_OPTIONS"
echo ""
make $MAKE_OPTIONS
res=$?

test "$res" = "0" || {
    echo "build failed return value $res"
}
test -f shell/$shell || {
    echo "avmshell is missing, build failed"
    cd $basedir/core
    mv avmplusVersion.h.orig avmplusVersion.h
    endSilent
    exit 1
}

# check to see if we're running under Jenkins
if [ "$JENKINS_HOME" != "" ]; then
    shellpath="shell"
    shellname="$filename$shell_extension"
else
    shellpath="$buildsdir/${change}-${changeid}/$platform"
    shellname="$filename$shell_extension"
    mkdir -p $shellpath
    chmod 777 $shellpath
fi # end Jenkins check

mv shell/$shell $shellpath/$shellname
chmod 777 $shellpath/$shellname

# Check to see if it is possible to run the generated shell, we could be cross compiling
# Look for the version string since calling the shell without an ABC will have a non-zero exitcode
echo ""
echo "*******************************************************************************"
echo "shell compiled with these features:"
avmfeatures=`$shellpath/$shellname -Dversion | grep AVM | sed 's/\;/ /g' | sed 's/features //g'`
for i in ${avmfeatures}; do
    echo $i
done
echo ""
failbuild=0
# bz: 779944, only run feature validation if shell can be executed natively
if [[ "$avmfeatures" != "" ]]; then
    for i in ${features}; do
        feature_ok=0
        if [[ $i == +* ]]; then
            echo "Make sure that ${i:1} is enabled"
            for feat in ${avmfeatures}; do
                if [ "$feat" == "${i:1}" ]; then
                    feature_ok=1
                    break
                fi
            done
            if [ $feature_ok != 1 ]; then
                echo "---> FAIL"
                failbuild=1
            else
                echo "---> PASS"
            fi
        fi
        if [[ $i == -* ]]; then
            feature_ok=1
            echo "Make sure that ${i:1} is NOT enabled"
            for feat in ${avmfeatures}; do
                if [ "$feat" == "${i:1}" ]; then
                    feature_ok=0
                    break
                fi
            done
            if [ $feature_ok == 0 ]; then
                echo "---> FAIL"
                failbuild=1
            else
                echo "---> PASS"
            fi
        fi
        echo ""
    done
fi

if [ $failbuild == 1 ]; then
    echo "message: feature check FAILED"
    cd $basedir/core
    mv avmplusVersion.h.orig avmplusVersion.h
    # Remove the binary since we have determined that it is NOT valid
    rm $shellpath/$shellname
    endSilent
    exit 1
fi # end feature check
echo "*******************************************************************************"

cd $basedir/core
mv avmplusVersion.h.orig avmplusVersion.h

# Post the build shell to ASTEAM
if ${upload}; then
    cd $basedir/build/buildbot/slaves/scripts/
    ../all/util-upload-ftp-asteam.sh $shellpath/$shellname $ftp_asteam/$branch/$shellname
    ret=$?
    if [ "$ret" != "0" ]; then
        echo "Uploading of $platform/$shellname failed"
        exit 1
    fi
fi

# only delete if not running under Jenkins
if [ "$JENKINS_HOME" == "" ]; then
    rm -rf $basedir/${compiledir}
fi # end Jenkins check

echo "build succeeded"

endSilent

exit 0

