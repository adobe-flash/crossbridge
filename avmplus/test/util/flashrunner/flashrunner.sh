#!/bin/sh
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

test "$1" = "" && {
    echo "flashrunner"
    exit 0
}
while true
do
    abc=$1
    echo $abc | grep '.abc' > /dev/null
    test "$?" = "0" && break
    _args="$_args $1"
    shift
    test "$#" = "0" && {
        echo found no abc in $_args
        exit 1
    }
done

test "$debug" = "1" &&
    args="-m pdb"
base=`dirname $0`
uname | grep CYG > /dev/null
if [ "$?" = "0" ]
then
    base=`cygpath -m -a $base`
    test "$PYTHONWIN" = "" && {
        echo "must set PYTHONWIN to windows native python."
        exit 1
    }
else
    test "$PYTHONWIN" = "" && {
        PYTHONWIN=/usr/bin/python
    }
fi

test "$abc" = "" && {
    echo "flashrunner test.abc"
    exit 1
}
name=`basename $abc`
test -d $base/working || {
    mkdir $base/working
}
test -f $abc || {
    echo "ERROR $1 file not found"
    exit 1
}
cp -p $abc $base/working
cd $base/working
$PYTHONWIN $args ../flashrunner.py "$name"
