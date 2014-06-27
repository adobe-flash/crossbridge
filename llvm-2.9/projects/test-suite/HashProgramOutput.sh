#!/bin/sh

if [ $# != 1 ]; then
  echo "$0 <output path>"
  exit 1
fi

md5cmd=$(which md5sum)
is_md5sum=1
if [ ! -x "$md5cmd" ]; then
    md5cmd=$(which md5)
    is_md5sum=0
    if [ ! -x "$md5cmd" ]; then
        echo "error: unable to find either 'md5sum' or 'md5'"
        exit 1
    fi
fi

mv $1 $1.bak
if [ $is_md5sum = "0" ]; then
    $md5cmd < $1.bak > $1
else
    $md5cmd < $1.bak | cut -d' ' -f 1 > $1
fi
rm -f $1.bak
