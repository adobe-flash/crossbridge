#!/bin/sh
#usage: find svn -name *.c -exec sh rename.sh {} . \;
FILE=`echo "$1" | sed -e 's/svn\///' | sed -e 's/\//_/g'`
cp $1 $FILE
