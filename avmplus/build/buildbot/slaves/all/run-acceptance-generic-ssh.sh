#!/bin/bash
# -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- #
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) #
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
    echo "usage: run-acceptance-generic-ssh.sh <change> <filename> <vmargs> <config> <scriptargs>"
    echo "  <change>     build number of shell to test"
    echo "  <filename>   name of the shell, do not include file extenstion"
    echo "  <vmargs>     vmargs to be passed or empty quoted string"
    echo "  <config>     custom config string to be passed to runtests.py"
    echo "               or an empty string"
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

# assign the remaining positional params to scriptargs
shift 4
scriptargs=$*


export shell=$filename$shell_extension

# If running under Jenkins, avm and asc come from upstream jobs via the
# "copy artifact" plugin and should not be downloaded via ftp
if [ "$JENKINS_HOME" = "" ]; then
    ##
    # Download the AVMSHELL if it does not exist
    ##
    download_shell $shell

    ##
    # Download the latest asc.jar if it does not exist
    ##
    download_asc

    export native_shell=${buildsdir}/${change}-${changeid}/${platform}/$shell

fi # end Jenkins check

export AVM=$basedir/build/buildbot/slaves/all/ssh-shell.sh
echo AVM=$AVM
test -f $AVM || {
    echo "ERROR: $AVM not found"
    exit 1
}

echo ""
echo "Missing media will be compiled using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

echo "setup ${native_shell}"
../all/ssh-shell-deployer.sh ${change} ${native_shell}
res=$?
test "$res" = "0" || {
    echo "message: setup failed"
    exit 1
}

###
# check for running avmshell processes
###
echo "checking for running avmshell processes"
../all/util-process-clean-ssh.sh


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

if [ "$silent" == "true" ] && [ "$internal_repo" == "true" ]; then
    logfile=`echo acceptance-$shell$vmargs.log | sed 's/ //g' | sed 's/\.exe//g'`
    silentoptions="-l $logfile --summaryonly"
fi

# threads is set in environment.sh
test "$threads" = "" || {
    scriptargs="--passthreadid --threads=$threads $scriptargs"
}
if [ "$config" != "" ]
then
    echo "message: $PYTHON_RUNTESTS ./runtests.py --vmargs=\"${vmargs}\" --config=${config} --notimecheck ${scriptargs} ${silentoptions}"
    $PYTHON_RUNTESTS ./runtests.py  --vmargs="${vmargs}" --config=${config} --notimecheck ${scriptargs} ${silentoptions}
else
    echo "message: $PYTHON_RUNTESTS ./runtests.py --vmargs=\"${vmargs}\" --notimecheck ${scriptargs} ${silentoptions}"
    $PYTHON_RUNTESTS ./runtests.py  --vmargs="${vmargs}" --notimecheck ${scriptargs} ${silentoptions}
fi

if [ "$silent" == "true" ] && [ "$internal_repo" == "true" ]; then
    # upload log to asteam
    $basedir/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $logfile $ftp_asteam/$branch/${change}-${changeid}/$platform/
    ret=$?
    if [ "$ret" != "0" ]; then
        echo "Uploading of $logfile failed"
        exit 1
    fi
    echo "Acceptance logfile can be found here: http://asteam.corp.adobe.com/builds/$branch/${change}-${changeid}/$platform/$logfile"
    echo "url: http://asteam.corp.adobe.com/builds/$branch/${change}-${changeid}/$platform/$logfile logfile"
fi

###
# check for running avmshell processes
###
 echo "checking for running avmshell processes"
 cd $basedir/build/buildbot/slaves/scripts
 ../all/util-process-clean-ssh.sh

if [ "$JENKINS_HOME" = "" ]; then
    ##
    # Ensure that the system is torn down and clean
    ##
    cd $basedir/build/buildbot/slaves/scripts
    ../all/util-acceptance-teardown.sh
fi

exit 0


