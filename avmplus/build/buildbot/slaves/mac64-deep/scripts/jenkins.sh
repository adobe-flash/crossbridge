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

shell_suffix="_64"
shell_extension=""
base_configure_args="--target=x86_64-darwin --mac-sdk=106"
base_features="+AVMSYSTEM_64BIT +AVMSYSTEM_AMD64"

##
# Get the shell binaries. Have to do it this way as the copy artifact does a hard
# fail if you try and get a binary from a project that doesn't exists, such as what
# it would ask for for the linux-arm builds, "linux-arm-test".
rm -rf $WS/objdir*
mkdir -p $WS/objdir/shell/
cd $WS/objdir/shell/
wget ${build_url}platform=mac64-intel/artifact/objdir/shell/avmshell${shell_suffix}${shell_extension}
chmod 777 avmshell${shell_suffix}${shell_extension}
wget ${build_url}platform=mac64-intel/artifact/objdir/shell/avmshell_s${shell_suffix}${shell_extension}
chmod 777 avmshell_s${shell_suffix}${shell_extension}
wget ${build_url}platform=mac64-intel/artifact/objdir/shell/avmshell_d${shell_suffix}${shell_extension}
chmod 777 avmshell_d${shell_suffix}${shell_extension}
wget ${build_url}platform=mac64-intel/artifact/objdir/shell/avmshell_sd${shell_suffix}${shell_extension}
chmod 777 avmshell_sd${shell_suffix}${shell_extension}




################################################################################
################################################################################
#
#   WORDCODE Compilation and Testing
#
################################################################################
################################################################################

##############################
# Wordcode Release compilation
##############################
shell_name="avmshell_wordcode"
configure_args="${base_configure_args} --enable-wordcode-interp"
compiledir="objdir-release-wordcode"
features="${base_features} +AVMFEATURE_WORDCODE_INTERP"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "Wordcode release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#####################################
# Wordcode Debug-Debugger compilation
#####################################
shell_name="avmshell_sd_wordcode"
configure_args="${base_configure_args} --enable-debug --enable-debugger --enable-wordcode-interp"
compiledir="objdir-debugdebugger-wordcode"
features="${base_features} +AVMFEATURE_WORDCODE_INTERP"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "Wordcode debug-debugger compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#############################
# Wordcode Release Acceptance
#############################
export shell_name=avmshell_wordcode
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-wordcode"
export vmargs="-Dinterp"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

####################################
# Wordcode Debug Debugger Acceptance
####################################
export shell_name=avmshell_sd_wordcode
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-debugger-wordcode"
export vmargs="-Dinterp"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
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

##########################
# ReleaseDebugger selftest
##########################
cd $WS
export shell_name=avmshell_s
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-debugger-selftest"

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

########################
# DebugDebugger selftest
########################
cd $WS
export shell_name=avmshell_sd
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-debugger-selftest"

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

#####################################
# OSR Release compilation
#####################################
shell_name="avmshell_osr"
configure_args="${base_configure_args} --enable-osr"
compiledir="objdir-release-osr"
features="${base_features} +AVMFEATURE_OSR"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "OSR release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#####################################
# OSR Debug compilation
#####################################
shell_name="avmshell_d_osr"
configure_args="${base_configure_args} --enable-osr --enable-debug"
compiledir="objdir-debug-osr"
features="${base_features} +AVMFEATURE_OSR"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "OSR release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#############################
# OSR=17 Release Acceptance
#############################
export shell_name=avmshell_osr
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-osr=17"
export vmargs="-osr=17"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
export shell_name=avmshell_osr
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-osr=0"
export vmargs="-osr=0"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
export shell_name=avmshell_d_osr
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-osr=17"
export vmargs="-osr=17"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
export shell_name=avmshell_d_osr
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-osr=0"
export vmargs="-osr=0"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
#   AIR Configuration Compilation and Testing
#
################################################################################
################################################################################

##############################
# AIR Release-Debugger compilation
##############################
shell_name="avmshell_air"
configure_args="${base_configure_args}  --enable-override-global-new --enable-use-system-malloc --enable-debugger"
compiledir="objdir-releasedebugger-air"
features="${base_features} +AVMFEATURE_DEBUGGER +AVMFEATURE_OVERRIDE_GLOBAL_NEW +AVMFEATURE_USE_SYSTEM_MALLOC"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "AIR release-debugger compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#############################
# AIR Release-Debugger Acceptance
#############################
export shell_name=avmshell_air
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="releasedebugger-air"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
#   Heap Graph Compilation and Testing
#
################################################################################
################################################################################

##############################
# Heap Graph Release compilation
##############################
shell_name="avmshell_heapgraph"
configure_args="${base_configure_args} --enable-heap-graph"
compiledir="objdir-release-heapgraph"
features="${base_features} +AVMTWEAK_HEAP_GRAPH"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "Release heap graph compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#############################
# Heap Graph Release Acceptance
#############################
export shell_name=avmshell_heapgraph
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-heapgraph"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
#   Valgrind Compilation and Testing
#
################################################################################
################################################################################

##############################
# Valgrind Release-Debugger compilation
##############################
shell_name="avmshell_s_valgrind"
configure_args="${base_configure_args} --enable-debugger --enable-valgrind"
compiledir="objdir-release-valgrind"
features="${base_features} +AVMFEATURE_DEBUGGER +AVMFEATURE_VALGRIND"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "Valgrind release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#############################
#  Valgrind Release-Debugger Acceptance
#############################
export shell_name=avmshell_s_valgrind
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="releasedebugger-valgrind"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
# NOTE: The machine that this currently runs on is a little slow so we need
# to increase the timeout for running the valgrind suite:
#     ecma3/Unicode/u3400_CJKUnifiedIdeographsExtensionA.as   time 678.3
#     ecma3/Unicode/uE000_PrivateUseArea.as   time 665.2
#     mmgc/pauseForGCIfCollectionImminent.as   time 887.2
#     mops/mops.abc_   time 981.6
#     mops/mops_basics.as   time 994.1
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--valgrind --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=1100 $suite"
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
#  Greedy Release Acceptance
#############################
export shell_name=avmshell
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-Dgreedy"
export vmargs="-Dgreedy"
export scriptargs="--timeout=180 --random --threads=1"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "${scriptargs} --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# GC Threshold Release Acceptance
#############################
export shell_name=avmshell
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-gcthreshold"
export vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# GC Threshold Debug-Debugger Acceptance
#############################
export shell_name=avmshell_sd
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debugdebugger-gcthreshold"
export vmargs="-Dgcthreshold 128 -load 1.05,1,1.05,5,1.05,20"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
#   Verifier Testing
#
################################################################################
################################################################################

#############################
# VerifyAll Debug Debugger Acceptance
#############################
export shell_name=avmshell_sd
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debugdebugger-verifyall"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--verify --timeout=300 --random --threads=1 --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# VerifyOnly Debug Debugger Acceptance
#############################
export shell_name=avmshell_sd
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debugdebugger-verifyonly"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--verifyonly --timeout=300 --random --threads=1 --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --testtimeout=300 $suite"
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
#   JIT Testing
#
################################################################################
################################################################################

#############################
# Ojit Debug Acceptance
#############################
export shell_name=avmshell_d
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debug-jit"
export vmargs="-Ojit"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
#   Halfmoon JIT Compilation and Testing
#
################################################################################
################################################################################

##############################
# Halfmoon Release compilation
##############################
shell_name="avmshell_halfmoon"
configure_args="${base_configure_args} --enable-halfmoon"
compiledir="objdir-release-halfmoon"
features="${base_features} +AVMFEATURE_HALFMOON"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "Halfmoon release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi


##############################
# Halfmoon Debug-Debugger compilation
##############################
shell_name="avmshell_sd_halfmoon"
configure_args="${base_configure_args} --enable-halfmoon --enable-debug --enable-debugger"
compiledir="objdir-debugdebugger-halfmoon"
features="${base_features} +AVMFEATURE_HALFMOON +AVMFEATURE_DEBUGGER"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "Halfmoon release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi


#############################
# Halfmoon Release Acceptance
#############################
export shell_name=avmshell_halfmoon
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="release-halfmoon"
export vmargs="-Dhalfmoon"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# Halfmoon Debug-Debugger Acceptance
#############################
export shell_name=avmshell_sd_halfmoon
export AVM="$WS/objdir/shell/$shell_name$shell_suffix"
export mode="debugdebugger-halfmoon"
export vmargs="-Dhalfmoon"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
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
#   MISC Testing
#
################################################################################
################################################################################

#############################
# ESC Testing
#############################
unset AVM
cd $WS/build/buildbot/slaves/scripts
# The ESC script makes a hard assumption that the shell will be called
# "avmshell", but on 64bit platform it is avmshell_64, so simply make a copy
cp $WS/objdir/shell/avmshell_64 $WS/objdir/shell/avmshell
../all/run-release-esc.sh "$rev_id"
if [ "$?" != "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

exit ${exitcode}



