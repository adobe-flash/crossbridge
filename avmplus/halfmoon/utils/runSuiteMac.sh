#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

root=`hg root`

if [ -d $root/third_party/avmplus ]; then
	root=$root/third_party/avmplus
fi

BUILTIN=$root/generated/builtin.abc

if [ ! -f "$ASC" ]; then
	# try utils/asc.jar
	ASC=$root/utils/asc.jar
fi
if [ ! -f "$ASC" ]; then
	# try tools/asc/asc.jar
	ASC=$root/../tools/asc/asc.jar
fi

command=$1; shift
dir=$1; shift

usage() {
    echo "usage: runSuiteMac.sh [compile|run] dir [avmpath]"
}

# Get cpu count.  This works on osx, TBD for others.
ncpu=`sysctl -n hw.ncpu`

if [ "$command" = "" ] || [ "$dir" = "" ] ; then
  usage && exit 1
fi

case $command in
compile)
	# remove and recompile all the as files in $dir
	find $dir -name \*.abc | xargs rm
	find $dir -name \*.as |
		xargs -P$ncpu -n 1 java -jar $ASC -AS3 -import $BUILTIN 2>&1 |
		grep --line-buffered abc
	;;
run)
	avm=$1; shift
	if [ "$avm" = "" ] ; then
	  usage && exit 1
	fi
	python $root/halfmoon/utils/runTestSuite.py $avm $dir
	;;
*)
    usage && exit 1
	;;
esac
