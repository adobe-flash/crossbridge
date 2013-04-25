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

# silence output if silent=true (function defined in environment.sh)
logfile=release-sizereport.log
beginSilent

##
# Make sure that there are no left over directories from previous compile
##
cd $basedir
test -d objdir && {
    echo Remove directory $basedir/objdir
    rm -rf $basedir/objdir
}

mkdir objdir

cd objdir

python ../configure.py --disable-eval --disable-selftest

echo ""
echo "*******************************************************************************"
echo "Makefile:"
echo ""
cat Makefile
echo ""
echo "*******************************************************************************"
topsrcdir=`grep topsrcdir= Makefile | awk -F"=" '{print $2}'`
CXX=`grep CXX= Makefile | awk -F"=" '{print $2}'| sed 's/(/{/' | sed 's/)/}/' | sed 's/-nologo//'`
echo ""
echo compiler version: 
eval ${CXX} --version
echo ""
echo "*******************************************************************************"
echo ""

make $MAKE_OPTIONS clean
make $MAKE_OPTIONS 
res=$?

test "$res" = "0" || {
    echo "build failed return value $res"
    endSilent
    exit $res
}
test -f "$basedir/objdir/shell/avmshell.map" || {
    echo "avmshell.map file was not created"
    endSilent
    exit 1
}

endSilent
cd $basedir/utils
python ./sizereport.py --vmversion=$change --product=${branch} --socketlog --prefix=message: --map=../objdir/shell/avmshell.map

echo url: "http://tamarin-builds.mozilla.org/report/index.cfm?mode=size&rollupname=sizereport&hostip=10.60.147.240&product1=tamarin-redux&product2=tamarin-redux&productBaseline=tamarin-redux&baselineBuild=1094&topBuild=${change}" size report

# We have had some problems in the past with deleting the objdir directory after running the 
# sizereport, give the script a couple of seconds to make sure that the python process completely
# shutsdown prior to trying to remove the directory
sleep 10

cd $basedir
test -d objdir && {
    echo Remove directory $basedir/objdir
    rm -rf $basedir/objdir
}

