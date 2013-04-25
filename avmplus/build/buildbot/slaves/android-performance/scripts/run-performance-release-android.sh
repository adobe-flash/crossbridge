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
# Run any slave specific code PRE performance
##
. ./run-performance-pre.sh


##
# Test for adb in path, if not found choosecombo must be run
##
adb=`which adb`
test "$adb" == "" && {
    echo "ERROR: android sdk is not setup, run: cd /Volumes/android/device; . ./build/envsetup.sh; choosecombo"
    exit 1
}


##
# Download the AVMSHELL if it does not exist
##
download_shell $shell_release


##
# Download the latest asc.jar if it does not exist
##
download_asc

##
# Install the AVMSHELL on the device
##
echo "Setting up the device with build #$change"
adb push $buildsdir/$change-${changeid}/$platform/$shell_release /data/app


echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""

export AVM=$basedir/platform/android/android_shell.py



cd $basedir/test/performance
find ./ -name "*.abc" -exec rm {} \;

measurememory="false"
result="0"
resultmessage=""

# If a second parameter (after revision is given) then only run those tests
if [ $2 ]
then
    echo ""
    echo "===========   Custom Run: $2   ==========="
    # determine what iter to apply based on testname
    if [[ $2 == jsbench* ]] ; then
        vmargs=""
        iter=3
    elif [[ $2 == sunspider* ]] ; then
        vmargs=""
        iter=10
    elif [[ $2 == v8* ]] ; then
        vmargs=""
        iter=10
    else
        vmargs=""
        iter=3
    fi
    echo "python ./runtests.py -r $branch -k -f -i $iter --vmargs='$vmargs' $2"
    python ./runtests.py -r $branch -k -f -i $iter --vmargs="$vmargs" $2
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$2 time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --memory -r $branch -k -f -i 1 $2
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \n$2 memory test run failed. " 
        }
    }
else
# standard performance run
    echo ""
    echo "===========   JSBench   ==========="
    python ./runtests.py --config=jsbench -r $branch -k -f -i 3
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="jsbench time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=jsbench --memory -r $branch -k -f -i 1
        test "$?" = "0" || { 
           result="1"; 
            resultmessage="$resultmessage \njsbench memory test run failed. " 
        }
    }
    echo ""
    echo "===========   Misc   ==========="
    python ./runtests.py --config=misc-mobile -r $branch -k -f -i 3
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \nmisc time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=misc --memory -r $branch -k -f -i 3
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \nmisc memory test run failed. " 
        }
    }
    echo ""
    echo "===========   SunSpider   ==========="
    python ./runtests.py --config=sunspider-mobile -r $branch -k -f -i 10
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \nsunspider time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=sunspider --memory -r $branch -k -f -i 3
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \nsunspider memory test run failed. " 
        }
    }
    
    echo ""
    echo "===========   V8   ==========="
    python ./runtests.py --config=v8 -r $branch -k -f -i 3 --larger
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \nv8 time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=v8 --memory -r $branch -k -f -i 3
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \nv8 memory test run failed. " 
        }
    }
    
    echo ""
    echo "===========   MMGC   ==========="
    python ./runtests.py --config=mmgc -r $branch -k -f -i 3 --vmargs=""
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \nmmgc time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=mmgc --memory -r $branch -k -f -i 1
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \nmmgc memory test run failed. " 
        }
    }    
fi # second parameter

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
cd $basedir/../scripts/
. ./run-performance-post.sh

test "$result" = "0" && resultmessage="performance tests passed"
echo -e "message: $resultmessage"
exit $result