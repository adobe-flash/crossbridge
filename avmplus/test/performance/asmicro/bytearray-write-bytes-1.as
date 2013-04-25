/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Write a small number of aligned bytes from ByteArray into ByteArray.  Compare bytearray-write-bytes-2.";
include "driver.as"

function bytearray_write_bytes(iter: int, a: ByteArray): uint
{
    // Sanity
    if (a.length != 8000 * 5)
        throw "Error in setup";

    var tmp1:ByteArray = new ByteArray;  tmp1.length = 8000;
    var tmp2:ByteArray = new ByteArray;  tmp2.length = 8000;
    var tmp3:ByteArray = new ByteArray;  tmp3.length = 8000;
    var tmp4:ByteArray = new ByteArray;  tmp4.length = 8000;
    var tmp5:ByteArray = new ByteArray;  tmp5.length = 8000;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        tmp1.position = 0;
        tmp2.position = 0;
        tmp3.position = 0;
        tmp4.position = 0;
        tmp5.position = 0;
        var k:int=0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            tmp1.writeBytes(a, k, 8);
            tmp2.writeBytes(a, k, 8);
            tmp3.writeBytes(a, k, 8);
            tmp4.writeBytes(a, k, 8);
            tmp5.writeBytes(a, k, 8);
            k += 8;
        }
    }

    // Sanity
    tmp1.position = 32;
    var n : Number = tmp1.readDouble();
    if (n != Math.PI || tmp1.length != 8000)
        throw "Error in copying: length=" + tmp.length + ", sample=" + n;

    return tmp1.position+tmp2.position+tmp3.position+tmp4.position+tmp5.position;
}

var bai: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 5000 ; i++ )
        bai.writeDouble(Math.PI);
 })();
TEST(function () { bytearray_write_bytes(250, bai); }, "bytearray-write-bytes-1");
