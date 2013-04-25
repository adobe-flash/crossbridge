#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh


adb=$basedir/platform/android/adb_proxy.py
echo
echo "+++++++++++++++++++++++++++++++++++++++++"
echo "Ensure that the system is clean and ready"
echo "+++++++++++++++++++++++++++++++++++++++++"
echo

#
# kill any rogue adb processes running on host
#
echo "========================================="
echo "kill any rogue adb processes running on host..."
adbp=`ps -ef | grep adb | grep android_runner | awk '{print $2}'`
for p in $adbp
do
    echo "Found adb process: `ps -ef | grep adb | grep android_runner | grep $p`"
    echo "Found running adb process: $p"
    kill $p
done
echo ""
sleep 2


##
# get list of connected devices
##
echo "Connected devices:"
${adb} devices
devices=`${adb} devices | awk '{ if ( $2  ~ /device/ ) print $1 }'`


#
# for each device look for avmshell processes and kill them
#
echo "========================================="
echo "rebooting devices..."
for device in $devices
do
    echo "Rebooting device - $device"
    ${adb} -s $device reboot
done
sleep 60
echo ""

#
# for each device look for avmshell processes and kill them
#

echo "========================================="
echo "cleaning up any avmshell processes on device..."
for device in $devices
do
    echo "Looking for avmshell processes on device - $device"
    ps=`${adb} -s $device shell ps | grep avmshell | awk '{print $2}'`
    for p in $ps
    do
	echo "found avmshell process: `${adb} -s $device shell ps | grep avmshell | grep $p`"
        echo "killing $p"
        ${adb} -s $device shell kill $p
    done
done
echo ""

#
# remove old abc and shells from the device
#
echo "========================================="
echo "cleanup up /data/local/tamarin directories on device..."
for device in $devices
do
    echo "cleaning up device - $device"
    ${adb} -s $device shell rm -r /data/local/tamarin
    ${adb} -s $device shell mkdir /data/local/tamarin
done
echo ""


echo "cleanup finished"

echo; echo;


