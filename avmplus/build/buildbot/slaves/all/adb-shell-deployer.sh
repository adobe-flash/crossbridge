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

filename=$2
test "$filename" = "" && {
    filename=$shell_release
}

adb=$basedir/platform/android/adb_proxy.py
# Determine the number of devices attached to the machine and their ids
adboutput=`${adb} devices`
deviceids=''
devicecount=0
IFS=$'\n'
for i in ${adboutput}; do
    if echo $i | grep -q 'device$'; then
        deviceids="${deviceids} `echo $i | awk '{print $1}'`"
        let devicecount=devicecount+1
    fi
done
unset IFS

echo ""
echo "Found ${devicecount} devices"
echo""
echo "Installing $filename"

for device in ${deviceids};do

    # Copy the shell and runner script to the device
    ${adb} -s ${device} push $filename /data/local/tamarin/avmshell 2> /dev/null
    ${adb} -s ${device} shell 'chmod 777 /data/local/tamarin/avmshell' 2> /dev/null
    ${adb} -s ${device} push ${basedir}/platform/android/android_runner.sh /data/local/tamarin 2> /dev/null
    ${adb} -s ${device} shell 'chmod 777 /data/local/tamarin/android_runner.sh' 2> /dev/null

    # Make sure that the version running on the device is the expected revision
    adb -s ${device} shell 'cd /data/local/tamarin; ./avmshell' > /tmp/stdout${device}
    # Verify that the shell was successfully deployed
    # Remove CR from the stdout as they really mess up shell commands on mac,
    # causing really weird output, characters getting moved around
    deploy_rev=`tr -d '\r' < /tmp/stdout${device} | grep "avmplus shell" | awk '{print $NF}'`
    if [ "$change" != "${deploy_rev%:*}" ] || [ "$changeid" != "${deploy_rev#*:}" ];
    then
        echo $0 FAILED!!!
        echo "requested build $change:$changeid is not what is deployed ${deploy_rev%:*}:${deploy_rev#*:}"
        exit 1
    fi
    echo "device ${device} setup with ${filename}"
done

exit
