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



##
# Update the version string
##
. ../all/util-update-version.sh


##
# Make sure that there are no left over directories from previous compile
##
cd $basedir/platform/win32
test -d obj_9 && {
    echo Remove directory $basedir/platform/win32/obj_9
    rm -rf obj_9
}


test -f build.out && rm -f build.out

echo "devenv avmplus2008.sln /clean VTune"
devenv avmplus2008.sln /clean VTune

echo "devenv avmplus2008.sln /rebuild VTune /out build.out"
devenv avmplus2008.sln /rebuild VTune /out build.out
res=$?
cat build.out
rm build.out
test "$res" = "0" || {
    echo "build failed return value $res"
}
test -f obj_9/shell/VTune/avm.exe || {
    echo "avm is missing, build failed"
    cd $basedir/core
    mv avmplusVersion.h.orig avmplusVersion.h
    exit 1
}


mkdir -p $buildsdir/${change}-${changeid}/$platform
chmod 777 $buildsdir/${change}-${changeid}/$platform
cp obj_9/shell/VTune/avm.exe $buildsdir/${change}-${changeid}/$platform/$shell_release_vtune
chmod 777 $buildsdir/${change}-${changeid}/$platform/$shell_release_vtune

echo ""
echo "*******************************************************************************"
echo "shell compiled with these features:"
features=`$buildsdir/${change}-${changeid}/$platform/$shell_release_vtune -Dversion | grep AVM | sed 's/\;/ /g' | sed 's/features //g'`
for i in ${features}; do
    echo feature: $i
done
echo ""
echo "*******************************************************************************"


cd $basedir/core
mv avmplusVersion.h.orig avmplusVersion.h

echo "build succeeded"
rm -rf $basedir/platform/win32/obj_9

exit 0
