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
# If we are running a sandbox build and the user specifies the build with the revision hash
# make sure that we stick with using the revision hash downstream as that is where we are
# going to find the necessary files on asteam, the ftp directory will have been created with the
# hash so that is how we must access it from the winrs client.
if [ "$changeid" == "$1" ]; then
   change=$changeid
fi

showhelp ()
{
    echo ""
    echo "usage: run-acceptance-generic-winrs.sh <change> <filename> <vmargs> <config> <scriptargs>"
    echo "  <change>     build number of shell to test"
    echo "  <filename>   name of the shell, do not include file extenstion"
    echo "  <vmargs>     vmargs to be passed or empty quoted string"
    echo "  <config>     custom config string to be passed to runtests.py"
    echo "               or an empty string"
    echo "  <deployargs> (optional) configuration arguments, 'skipdeploy' to not deploy the shell"
    echo "               and tests to the device"
    echo "  <scriptargs> (optional) additional runtests.py args to be passed, can be"
    echo "               args and test/directories to run, value is appended to call"
    exit 1
}

if [ "$#" -lt 4 ]
then
    echo "not enough args passed"
    showhelp
fi

filename=$2
vmargs=$3
config=$4
deploy=$5
# assign the remaining positional params to scriptargs
shift 5
scriptargs=$*


export shell=$filename$shell_extension

export AVM="$basedir/build/buildbot/slaves/all/winrs-shell.sh"
echo AVM=$AVM
test -f $AVM || {
    echo "ERROR: $AVM not found"
    exit 1
}
export AVM="bash $AVM"

echo ""
echo "Missing media will be compiled using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

if [ "$deploy" == "deploy" ]
then

    echo "running deploy on shell ${shell}"
    ../all/winrs-shell-deployer.sh ${change} ${shell} ${clean}
    res=$?
    test "$res" = "0" || {
        echo "message: setup failed"
        exit 1
    }

else
     echo "skipping deploy"
fi

echo ""
echo "*******************************************************************************"
echo AVM=$AVM
echo "`$AVM`"
echo ""
echo "shell compiled with these features:"
features=`$AVM -Dversion | grep AVM | sed 's/\;/ /g' | sed 's/features //g'`
for i in ${features}; do
    echo feature: $i
done
echo ""
echo "*******************************************************************************"
echo ""

cd $basedir/test/acceptance

scriptargs="--passthreadid --threads=$threads $scriptargs"

echo "message: $PYTHON_RUNTESTS ./runtests.py --vmargs=\"${vmargs}\" --config=${config} --notimecheck ${scriptargs}"
$PYTHON_RUNTESTS ./runtests.py  --vmargs="${vmargs}" --config=${config} --notimecheck ${scriptargs}


exit 0


