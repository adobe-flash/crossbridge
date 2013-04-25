/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read the 'length' field of an Array object, knowing its type is Array.  Compare {vector,object}-read-length-1."
include "driver.as"

function array_read_length(iter: int, x: Array): uint
{
    var n: uint = 0;
    for ( var j:int=0 ; j < iter ; j++ )
        for ( var i:int=0 ; i < x.length ; i++ )
            n += x.length;
    return n;
}

var a = new Array(1000);
TEST(function () { array_read_length(1000, a); }, "array-read-length-1");
