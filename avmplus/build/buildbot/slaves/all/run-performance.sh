#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


### PARAMS ###
# $1 = revision or revision:hash
# $2 = avmshell to run
# $3 = vmargs
# $4 = config (aka testsuite)
# $5 = iterations
# $6 = android-run?  (True or '', optional)
# $7 = logConfigAppend (additional text appended to vmargs to identify
#                       configuration in databse;  For example, any env 
#                       variables used by shell not yet codified as vmargs)

shellname=$2
vmargs=$3
config=$4
iterations=$5
android=$6
logConfigAppend=$7

##
# Bring in the environment variables
##
. ./environment.sh


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1

##
# Run any slave specific code PRE performance
##
. ./run-performance-pre.sh

if [ $android ]; then
    ##
    # Test for adb in path, if not found choosecombo must be run
    ##
    adb=`which adb`
    test "$adb" == "" && {
        echo "ERROR: android sdk is not setup.  Please see buildbot machines wiki for setup instructions."
        exit 1
    }
fi

# If running under Jenkins, avm and asc come from upstream jobs via the 
# "copy artifact" plugin and should not be downloaded via ftp
if [ "$JENKINS_HOME" = "" ]; then

    ##
    # Download the AVMSHELL if it does not exist
    ##
    download_shell $shellname


    ##
    # Download the latest asc.jar if it does not exist
    ##
    download_asc

    echo ""
    echo "Building ABC files using the following ASC version:"
    echo "`java -jar $ASC`"
    echo ""


    export AVM=$buildsdir/$change-${changeid}/$platform/$shellname

    if [ $android ]; then
        echo "setup $branch/${change}-${changeid}"
        ../all/adb-shell-deployer.sh ${change} ${buildsdir}/${change}-${changeid}/${platform}/$shellname
        res=$?
        test "$res" = "0" || {
            echo "message: setup failed"
            exit 1
        }
        export AVM=$basedir/platform/android/android_shell.py
    fi

    echo ""
    echo AVM=$AVM
    echo "`$AVM`"
    echo; echo "AVM built with the following options:"
    echo "`$AVM -Dversion`"
    echo ""

    ##
    # Ensure that the system is clean and ready
    ##
    cd $basedir/build/buildbot/slaves/scripts
    ../all/util-acceptance-clean.sh
fi

cd $basedir/test/performance

measurememory="false"
$AVM | grep "\-memstats" > /dev/null
test "$?" = "0" && {
  measurememory="true"
}


result="0"
resultmessage=""

echo ""
echo "===========   $config   ==========="
python ./runtests.py --config=$config -r $branch -k -f -i $iterations --vmargs="$vmargs" --repo="$repo" --logConfigAppend="$logConfigAppend"
test "$?" = "0" || { 
    result="1"; 
    resultmessage="$config time test run failed. " 
}

test "$measurememory" = "true" && {
    python ./runtests.py --config=$config --memory -r $branch -k -f -i 1 --vmargs="$vmargs" --repo="$repo" --logConfigAppend="$logConfigAppend"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \n$config memory test run failed. "
    }
}
echo ""
  

export memory_url="http://tamarin-builds.mozilla.org/report/index.cfm?hostip=${hostip}&config1=${configDefault}&config2=${configDefault}&configbaseline=${baselineConfigMemory}&baselineBuild=${baselineBuildMemory}&topBuild=${change}&product1=${branch}&product2=${branch}&productBaseline=${baselineBranchMemory}&raw=false&ratio=false&verbose=false&group=Desktop&metricsChecked=memory"
export time_url="http://tamarin-builds.mozilla.org/report/index.cfm?hostip=${hostip}&config1=${configDefault}&config2=${configDefault}&configbaseline=${baselineConfigTime}&baselineBuild=${baselineBuildTime}&topBuild=${change}&product1=${branch}&product2=${branch}&productBaseline=${baselineBranchTime}&raw=false&ratio=true&verbose=false&group=Desktop&metricsChecked=time"

echo ""
echo ""
echo "url: ${time_url} time report"
if [ "$measurememory" == "true" ]
then
    echo "url: ${memory_url} memory report"
fi

test -f results.log && rm results.log
wget -O results.log -q ${time_url}
retry="0"
perfchg=""
while true
 do
  test -f results.log && {
    sleep 10
    perfchg=`cat results.log | grep -m1 "perfchange_ratio_time:" | awk -F"perfchange_ratio_time:" '{print $2}' | awk '{print $1}'`
    break
  }
  retry=`echo $retry | awk '{ print $1+1 }'`
  test "$retry" = "20" && break
  sleep 1
done
echo "perfchange: ${perfchg}%"


##
# Run any slave specific code POST performance
##
cd $basedir/build/buildbot/slaves/scripts/
. ./run-performance-post.sh

if [ "$JENKINS_HOME" = "" ]; then
    ##
    # Ensure that the system is torn down and clean
    # Skip on android so that other avmshell processes on android host do not get
    # killed.
    ##
    if [ ! $android ]; then
        cd $basedir/build/buildbot/slaves/scripts
        ../all/util-acceptance-teardown.sh
    fi
fi

test "$result" = "0" && resultmessage="performance tests passed"
echo -e "message: $resultmessage"

exit $result