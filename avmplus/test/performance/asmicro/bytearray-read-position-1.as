/* -*- Mode: Java c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read the 'position' field of a ByteArray object, knowing its type is ByteArray."
include "driver.as"

function bytearray_read_position(iter: int, x: ByteArray): uint
{
    var n: uint = 0;
    var l: uint = x.length;
    for ( var j:int=0 ; j < iter ; j++ )
        for ( var i:int=0 ; i < l ; i++ )
            n += x.position;
    return n;
}

var a = new ByteArray();
a.length = 1000;
a.position = 37;

TEST(function () { bytearray_read_position(1000, a); }, "bytearray-read-position-1");
