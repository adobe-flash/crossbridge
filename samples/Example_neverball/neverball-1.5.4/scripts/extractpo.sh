#!/bin/sh
# This script extracts translatable messages from maps, sets and
# desktop files.
# Copyright (C) 2006 Jean Privat
#
# Part of the Neverball project
# http://icculus.org/neverball/
#
# NEVERBALL is  free software; you can redistribute  it and/or modify
# it under the  terms of the GNU General  Public License as published
# by the Free  Software Foundation; either version 2  of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
# MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
# General Public License for more details.

test -n "$1" || exit 1

LC_ALL=C
export LC_ALL

cd "$1"

DATA="data"
SETS="$DATA/sets.txt"
COURSES="$DATA/courses.txt"

print_msg() {
    string=$1
    file=$2
    comment=$3

    echo
    if [ "$comment" != "" ]; then
        echo "#. $comment"
    fi
    echo "#: $file"
    echo "msgid \"$string\""
    echo "msgstr \"\""
}

for i in $(cat "$SETS"); do
    i="$DATA/$i"

    # Only translate the two first lines
    head -n 2 $i | while read -r d; do
        msg=$(echo "$d" | sed 's/\\/\\\\/g')
        print_msg "$msg" "$i"
    done
done

for course in $(cat $COURSES); do
    course=$DATA/$course
    # Translate the second line
    print_msg "$(sed -n '2 { s/\\/\\\\/g; p; }' < $course)" $course
done

for i in $(find $DATA -name "*.map" | sort); do
    # Only translatable string is "message"
    grep -E "^\"message\"" "$i" | while read -r a b; do
        msg=$(echo "$b" | sed 's/\\/\\\\/g; s/^"\(.*\)"$/\1/')
        print_msg "$msg" "$i"
    done
done

for i in dist/*.desktop.in; do
    msg=$(grep '^Comment=' $i | sed 's/^Comment=//')
    if [ "$msg" != "" ]; then
        print_msg "$msg" "$i" "Desktop entry comment"
    fi
done

