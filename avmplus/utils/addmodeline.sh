#!/bin/sh
# -*- Mode: sh; indent-tabs-mode: nil; tab-width: 4 -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)

# usage: addmodeline.sh lang file1 file2 ...
#
# Where "lang" is one of:
#    "C", "c", "C++", "c++", "cpp", "cxx", "Java", "java",
#    "Python, "python", "py", "c_modeless".
#
# Add modeline(s) for language to top of each file.
#
# Not clever at all; e.g. leading #! lines, if any, must be restored manually.
#
# The "c_modeless" language avoids adding any mode at all, but adds a
# fallback modeline lines (taken from builtin.as) in a C comment.
# (See code for text of fallback modeline.)

# WARNINGS:
# 1. requires $file.orig does not exist (for all $file arguments).
# 2. clobbers leading #! lines.

# Features to-do list:
#
# 1. Rather than require explicit argument specifying language, infer
#    it from extension on filename.  (The case dispatch has been
#    written to accomodate this with relative ease.)
#
# 3. Safe-guard against files with #! in first line.
#    (At the very least, detect them and error in response;
#     preferably stash the #!-line away and restore to front
#     after modeline has been added.)

MODE_LANG=$1
shift

function add_modeline() {
    file=$1

    if [ ! -e $file ] ; then
        echo "Skipping $file, as it does not exist"
        exit 2
    fi
    if [ -e $file.orig ] ; then
        echo "Skipping $file; there is already a $file.orig present"
        exit 3
    fi

    egrep '^/\* +(-\*- +Mode:|vi: +set)' $file > /dev/null
    if [ $? = 0 ]; then
        echo "Skipping $file, it looks like it has a modeline already"
        return
    fi

    file -b $file | grep 'ASCII' > /dev/null
    if [ $? != 0 ]; then
        echo "Skipping $file; it is reported as non-ascii text"
        exit 5
    fi

    cp $file $file.orig

    case $MODE_LANG in
        C | c)
            add_modeline_c $file
            ;;
        C++ | c++ | cpp | cxx )
            add_modeline_cxx $file
            ;;
        Java | java )
            add_modeline_java $file
            ;;
        Python | python | py )
            add_modeline_python $file
            ;;
        c_modeless)
            add_modeline_c_modeless $file
            ;;
        * )
            echo "Unsupported language: $MODE_LANG"
            exit 1
    esac

    cat $file.orig >> $file
    rm $file.orig
}

function add_modeline_cxx() {
    file_new=$1
    cat > $file_new << "EOF"
/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
EOF
}

function add_modeline_c() {
    file_new=$1
    cat <<"EOF" > $file_new
/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
EOF
}

function add_modeline_java() {
    file_new=$1
    cat > $file_new << "EOF"
/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
EOF
}

function add_modeline_python() {
    file_new=$1
    cat > $file_new << "EOF"
#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
EOF
}

function add_modeline_c_modeless() {
    file_new=$1
    cat > $file_new << "EOF"
/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
EOF
}

for file in "$@"; do add_modeline $file ; done
