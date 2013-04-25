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
# Download the AVMSHELL if it does not exist
##
download_shell $shell_release


##
# Download the latest asc.jar if it does not exist
##
download_asc

echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""




export AVM=$buildsdir/$change-${changeid}/$platform/$shell_release


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


cd $basedir/test/performance
find ./ -name "*.abc" -exec rm {} \;

measurememory="false"
$AVM | grep "\-memstats" > /dev/null
test "$?" = "0" && {
  measurememory="true"
}

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
    python ./runtests.py -r $branch -k -f -i $iter --vmargs="$jit $vmargs" $2
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$2 time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py -r $branch -k -f -i 1 --memory --vmargs=$jit $2 
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \n$2 memory test run failed. " 
        }
    }
else
# standard performance run
    echo ""
    echo "===========   JSBench   ==========="
    python ./runtests.py --config=jsbench -r $branch -k -f -i 3 --vmargs="$jit"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="jsbench time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=jsbench -r $branch -k -f -i 1 --memory --vmargs=$jit
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="${resultmessage} \njsbench memory test run failed. " 
        }
    }
    
    echo ""
    echo "===========   Misc   ==========="
    python ./runtests.py --config=misc -r $branch -k -f -i 3 --vmargs="$jit"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="${resultmessage} \nmisc time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=misc -r $branch -k -f -i 1 --memory --vmargs=$jit
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="${resultmessage} \nmisc memory test run failed. " 
        }
    }
    
    echo ""
    echo "===========   SunSpider   ==========="
    python ./runtests.py --config=sunspider -r $branch -k -f -i 10 --vmargs="$jit"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="${resultmessage} \nsunspider time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=sunspider -r $branch -k -f -i 1 --memory --vmargs=$jit
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="${resultmessage} \nsunspider memory test run failed. " 
        }
    }
    
    echo ""
    echo "===========   V8   ==========="
    python ./runtests.py --config=v8 -r $branch -k -f -i 10 --larger --vmargs="$jit"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="${resultmessage} \nv8 time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=v8 -r $branch -k -f -i 1 --memory --vmargs=$jit
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="${resultmessage} \nv8 memory test run failed. " 
        }
    }
    
    echo ""
    echo "===========   MMGC   ==========="
    python ./runtests.py --config=mmgc -r $branch -k -f -i 5 --vmargs="$jit"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \nmmgc time test run failed. " 
    }
    test "$measurememory" = "true" && {
        python ./runtests.py --config=mmgc -r $branch -k -f -i 1 --memory --vmargs=$jit
        test "$?" = "0" || { 
            result="1"; 
            resultmessage="$resultmessage \nmmgc memory test run failed. " 
        }
    }    
fi

export memory_url="http://tamarin-builds.mozilla.org/report/index.cfm?hostip=${hostip}&config1=${configJIT}&config2=${configJIT}&configbaseline=${baselineConfigMemoryJIT}&baselineBuild=${baselineBuildMemoryJIT}&topBuild=${change}&product1=${branch}&product2=${branch}&productBaseline=${baselineBranchMemory}&raw=false&ratio=false&verbose=false&group=Desktop&metricsChecked=memory"
export time_url="http://tamarin-builds.mozilla.org/report/index.cfm?hostip=${hostip}&config1=${configJIT}&config2=${configJIT}&configbaseline=${baselineConfigTimeJIT}&baselineBuild=${baselineBuildTimeJIT}&topBuild=${change}&product1=${branch}&product2=${branch}&productBaseline=${baselineBranchTime}&raw=false&ratio=true&verbose=false&group=Desktop&metricsChecked=time"

echo ""
echo ""
echo "url: ${time_url} time report"
if [ "$branch" != "tamarin-tracing" ]
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
# Run any slave specific code PRE performance
##
cd $basedir/build/buildbot/slaves/scripts/
. ./run-performance-post.sh

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

test "$result" = "0" && resultmessage="performance tests passed"
echo -e "message: $resultmessage"
exit $result

