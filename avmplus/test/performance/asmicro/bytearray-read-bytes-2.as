/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read a large number of aligned bytes from ByteArray into ByteArray.  Compare bytearray-read-bytes-1.";
include "driver.as"

function bytearray_read_bytes(iter: int, a: ByteArray): uint
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
        a.position = 0;
        var k:int=0;
        for ( var j:int = 0 ; j < 10 ; j++ ) {
            a.readBytes(tmp1, k, 800);
            a.readBytes(tmp2, k, 800);
            a.readBytes(tmp3, k, 800);
            a.readBytes(tmp4, k, 800);
            a.readBytes(tmp5, k, 800);
            k += 800;
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
TEST(function () { bytearray_read_bytes(250, bai); }, "bytearray-read-bytes-2");
