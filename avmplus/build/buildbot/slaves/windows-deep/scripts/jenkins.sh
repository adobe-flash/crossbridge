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
shell_extension=".exe"
base_configure_args="--target=i686-windows"
base_features="+AVMSYSTEM_32BIT +AVMSYSTEM_IA32"

##
# Get the shell binaries. Have to do it this way as the copy artifact does a hard
# fail if you try and get a binary from a project that doesn't exists, such as what
# it would ask for for the linux-arm builds, "linux-arm-test".
rm -rf $WS/objdir*
mkdir -p $WS/objdir/shell/
cd $WS/objdir/shell/
wget ${build_url}platform=windows/artifact/objdir/shell/avmshell${shell_suffix}${shell_extension}
chmod 777 avmshell${shell_suffix}${shell_extension}
wget ${build_url}platform=windows/artifact/objdir/shell/avmshell_s${shell_suffix}${shell_extension}
chmod 777 avmshell_s${shell_suffix}${shell_extension}
wget ${build_url}platform=windows/artifact/objdir/shell/avmshell_d${shell_suffix}${shell_extension}
chmod 777 avmshell_d${shell_suffix}${shell_extension}
wget ${build_url}platform=windows/artifact/objdir/shell/avmshell_sd${shell_suffix}${shell_extension}
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
#   GC Testing
#
################################################################################
################################################################################

#############################
#  Greedy Release Acceptance
#############################
export shell_name=avmshell
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
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


# Bug: bz 750694, halfmoon fails to compile on windows
if [ "0" == "1" ]; then
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

# Bug: bz 750694, halfmoon fails to compile on windows
fi

################################################################################
################################################################################
#
#   Float Testing
#
################################################################################
################################################################################

#############################
# Float enabled compilation
#############################
##
# Need to prepare for float compilation, get float enabled asc.jar and rebuild generated code
cd $WS/build/buildbot/slaves/scripts
../all/float-pre.sh
if [ "$?" = "0" ]; then
    echo "preparation for float compilation passed"
else
    echo "error: preparation for float compilation failed"
    exitcode=1
fi

##############################
# Float Release compilation
##############################
shell_name="avmshell_float"
configure_args="${base_configure_args} --enable-float"
compiledir="objdir-release-float"
features="${base_features} +AVMFEATURE_FLOAT"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "float release compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

##############################
# Float Debug compilation
##############################
shell_name="avmshell_d_float"
configure_args="${base_configure_args} --enable-float --enable-debug"
compiledir="objdir-debug-float"
features="${base_features} +AVMFEATURE_FLOAT"
unset AVM
cd $WS/build/buildbot/slaves/scripts
../all/compile-generic.sh "$rev_id" "$configure_args" "${shell_name}${shell_suffix}" "false" "$features" "$compiledir"
if [ "$?" != "0" ]; then
    echo "float debug compilation failure."
    exitcode=1
fi
if [ -f $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} ]; then
    mv $WS/$compiledir/shell/$shell_name$shell_suffix${shell_extension} $WS/objdir/shell/$shell_name$shell_suffix${shell_extension}
fi

#############################
# Float Release Acceptance
#############################
export shell_name=avmshell_float
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
export mode="release-float"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
# Force a recompilation of the test media on the first run of acceptance since media needs to be recompiled
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--ascargs=-abcfuture --addtoconfig=-float -f --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

#############################
# Float Ojit Release Acceptance
#############################
export shell_name=avmshell_float
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
export mode="release-float-jit"
export vmargs="-Ojit"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--ascargs=-abcfuture --addtoconfig=-float --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi


#############################
# Float Debug Acceptance
#############################
export shell_name=avmshell_d_float
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
export mode="debug-float"
export vmargs=""
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--ascargs=-abcfuture --addtoconfig=-float --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi


#############################
# Float Ojit Debug Acceptance
#############################
export shell_name=avmshell_d_float
export AVM="$WS/objdir/shell/$shell_name$shell_suffix${shell_extension}"
export mode="debug-float-jit"
export vmargs="-Ojit"
cd $WS/build/buildbot/slaves/scripts
../all/run-acceptance-generic.sh "$rev_id" "$shell_name$shell_suffix" "$vmargs" "" "--ascargs=-abcfuture --addtoconfig=-float --showtimes --log runtests-$mode.txt --logjunit=acceptance-$mode.xml --threads=$threads --testtimeout=300 $suite"
failures=`grep "^failures" $WS/test/acceptance/runtests-$mode.txt | awk '{print $3}'`
if [ "$failures" = "0" ]; then
    echo "all tests passed"
else
    echo "error: found $failures FAILURES in acceptance tests"
    exitcode=1
fi

##
# Need to reset the env back to NON-float
cd $WS/build/buildbot/slaves/scripts
../all/float-post.sh
if [ "$?" = "0" ]; then
    echo "post preparation for float compilation passed"
else
    echo "error: post preparation for float compilation failed"
    exitcode=1
fi

##
# Update the testmedia back to NON-float
cd $WS/test/acceptance
echo "Removing old *.abc files"
# use absolute path so we get cygwin find and not windows find when on windows
/usr/bin/find ./ -name "*.abc" -exec rm {} \;
echo "Revert any changes in the acceptance directory."
echo "This will allow .abc files to actually be checked in."
p4 sync -f $WS/test/acceptance/as3/asc/import-with-float4-helper.abc
echo "Unzipping compiled test media"
unzip -o -q acceptance-tests-abcs.zip


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
../all/run-release-esc.sh "$rev_id"
if [ "$?" != "0" ]; then
    echo "ESC recompilation failure."
    exitcode=1
fi

##############################
# VTune Release compilation
##############################
cd $WS/build/buildbot/slaves/scripts
./build-vtune.sh "$rev_id"
if [ "$?" != "0" ]; then
    echo "VTune release compilation failure."
    exitcode=1
fi

exit ${exitcode}

