#!/bin/sh
# usage: find svn -name \*.h -exec sh header_rename.sh {} include/ \;
FILE=`basename "$1"`
cp $1 $FILE
