/* -*- Mode: Java c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

// Writing "position" comes into play with random access to the byte
// array: position has to be set before every read (if the read
// granularity is not simply a byte).  Writing position is essentially
// trivial, and will eventually be inlined by the jit.

var DESC = "Write the 'position' field of a ByteArray object, knowing its type is ByteArray."
include "driver.as"

function bytearray_write_position(iter: int, x: ByteArray): uint
{
    var l:uint = x.length;
    for ( var j:int=0 ; j < iter ; j++ )
        for ( var i:int=l-1 ; i >= 0 ; i--)
            x.position = i;
    return x.position;
}

var a = new ByteArray();
a.length = 1000;

TEST(function () { bytearray_write_position(1000, a); }, "bytearray-write-position-1");
