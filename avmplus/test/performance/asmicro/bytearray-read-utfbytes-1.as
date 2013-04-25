/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read short UTF string from un-prefixed ByteArray, store in local with type String.";
include "driver.as"

function bytearray_read_utfbytes(iter: int, a: ByteArray, b: ByteArray): int
{
    var tmp1:String;
    var tmp2:String;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        a.position = 0;
        b.position = 0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            var l:uint;
            l = a.readUnsignedShort();
            tmp1 = a.readUTFBytes(l);
            l = b.readUnsignedShort();
            tmp2 = b.readUTFBytes(l);
        }
    }
    return tmp1+tmp2;
}

var ba1: ByteArray = new ByteArray();
var ba2: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 1000 ; i++ ) {
        ba1.writeUTF("Bill");
        ba2.writeUTF("Ted");
    }
 })();
TEST(function () { bytearray_read_utfbytes(100, ba1, ba2); }, "bytearray-read-utfbytes-1");
