#!/bin/bash -x
# -*- Mode: Bash; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh


exitcode=0

shell_suffix=""
shell_extension=""
base_configure_args="--enable-arm-neon --arm-arch=armv7-a --target=arm-linux --enable-sys-root-dir=$TOOLCHAIN_ARM_LINUX/debian5"
base_features=""

##
# Get the shell binaries. Have to do it this way as the copy artifact does a hard
# fail if you try and get a binary from a project that doesn't exists, such as what
# it would ask for for the linux-arm builds, "linux-arm-test".
rm -rf $WS/objdir*
mkdir -p $WS/objdir/shell/
cd $WS/objdir/shell/
wget ${build_url}platform=linux-arm/artifact/objdir/shell/avmshell${shell_suffix}${shell_extension}
chmod 777 avmshell${shell_suffix}${shell_extension}
wget ${build_url}platform=linux-arm/artifact/objdir/shell/avmshell_d${shell_suffix}${shell_extension}
chmod 777 avmshell_d${shell_suffix}${shell_extension}


################################################################################
################################################################################
#
#   Acceptance Testing
#
################################################################################
################################################################################

#############################
# Softfloat Release Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-softfloat"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# Softfloat Deep Release Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-softfloat-deep"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release-deep" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# Deep Release Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-deep"
export vmargs="-Darm_arch 7 -Darm_vfp"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release-deep" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi


#############################
# Deep Release Interp Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-interp-deep"
export vmargs="-Dinterp"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release-Dinterp-deep" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi


#############################
# Debug Acceptance
#############################
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug"
export vmargs="-Darm_arch 7 -Darm_vfp"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-debug" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# JIT Debug Acceptance
#############################
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-jit"
export vmargs="-Darm_arch 7 -Darm_vfp -Ojit"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-debug-Ojit" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# Softfloat Debug Acceptance
#############################
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-softfloat"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-debug" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi


################################################################################
################################################################################
#
#   Self Tests
#
################################################################################
################################################################################

################
# Debug selftest
################
cd $WS
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-selftest"

test -f selftests-results-$mode.xml && rm selftests-results-$mode.xml
test -f selftests-results-$mode.txt && rm selftests-results-$mode.txt
$AVM -Dselftest > selftests-results-$mode.txt
cat selftests-results-$mode.txt
test/util/convertAcceptanceToJunit.py --type=selftests --ifile=$WS/selftests-results-$mode.txt --ofile=$WS/selftests-results-$mode.xml --toplevel=$mode-$platform
failures=`grep "fail" $WS/selftests-results-$mode.txt | wc -l | awk '{print $1}'`
passes=`grep "pass" $WS/selftests-results-$mode.txt | wc -l | awk '{print $1}'`
echo "passes  : $passes"
echo "failures: $failures"
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in selftests"
    exitcode=1
fi


################################################################################
################################################################################
#
#   OSR Compilation and Testing
#
################################################################################
################################################################################

#############################
# OSR=17 Release Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-osr=17"
export vmargs="-osr=17"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release-osr=17" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# OSR=0 Release Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-osr=0"
export vmargs="-osr=0"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release-osr=0" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# OSR=17 Debug Acceptance
#############################
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-osr=17"
export vmargs="-osr=17"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-debug-osr=17" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# OSR=0 Debug Acceptance
#############################
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-osr=0"
export vmargs="-osr=0"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-debug-osr=0" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi



################################################################################
################################################################################
#
#   GC Testing
#
################################################################################
################################################################################

#############################
# GC Threshold Release Acceptance
#############################
export shell_name=avmshell
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-gcthreshold"
export vmargs="-Darm_arch 7 -Darm_vfp -Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-release" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# GC Threshold Debug Acceptance
#############################
export shell_name=avmshell_d
export native_shell="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-gcthreshold"
export vmargs="-Darm_arch 7 -Darm_vfp -Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic-ssh.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "arm-lnx-tvm-debug" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi



exit ${exitcode}


