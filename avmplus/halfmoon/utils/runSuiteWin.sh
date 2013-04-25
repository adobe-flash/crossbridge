#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

root=`hg root`
ASC="$root/utils/asc.jar"
BUILTIN="$root/generated/builtin.abc"

command=$1; shift
dir=$1; shift

usage() {
    echo "usage: runSuiteWin.sh [compile|run] dir [avmpath]"
}

# Can't figure out how to find number of procs on mingw, so default 3
ncpu=3

if [ "$command" = "" ] || [ "$dir" = "" ] ; then
  usage && exit 1
fi

case $command in
compile)
	# remove and recompile all the as files in $dir
	find $dir -name \*.abc | xargs rm -f
	find $dir -name \*.as | 
		xargs -P$ncpu -n 1 java -jar $(cygpath -aw $ASC) -AS3 -import $(cygpath -aw $BUILTIN) 2>&1 | 
		grep abc
	;;
run)
	avm=$1; shift
	if [ "$avm" = "" ] ; then
	  usage && exit 1
	fi
    if [ "$1" != "" ] ; then
        ncpu=$1; shift
    fi
    python runTestSuite.py "$avm" "$dir" "$ncpu"
	;;
*)
    usage && exit 1
	;;
esac
