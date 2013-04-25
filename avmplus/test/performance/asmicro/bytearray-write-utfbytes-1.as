/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Write short string as un-prefixed UTFBytes to pre-sized ByteArray";
include "driver.as"

function bytearray_write_utfbytes(iter: int): uint
{
    const s1:String = "Bill";
    const s2:String = "Ted";
    const lim:int = 1000;
    var ba1: ByteArray = new ByteArray();
    ba1.length = s1.length*lim;
    var ba2: ByteArray = new ByteArray();
    ba2.length = s2.length*lim;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        ba1.position = 0;
        ba2.position = 0;
        for ( var j:int = 0 ; j < lim ; j++ ) {
            ba1.writeUTFBytes(s1);
            ba2.writeUTFBytes(s2);
        }
        if (ba1.length != s1.length*lim || ba1.position != ba1.length)
            throw "Error writing " + s1 + ": " + ba1.length + ", " + ba1.position;
        if (ba2.length != s2.length*lim || ba2.position != ba2.length)
            throw "Error writing " + s2 + ": " + ba2.length + ", " + ba2.position;
    }
    return ba1.length + ba2.length;
}

TEST(function () { bytearray_write_utfbytes(100); }, "bytearray-write-utfbytes-1");
