/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read aligned bigEndian uint from ByteArray, store in local with type uint.";
include "driver.as"

function bytearray_read_uint(iter: int, a: ByteArray): uint
{
    var tmp1:uint;
    var tmp2:uint;
    var tmp3:uint;
    var tmp4:uint;
    var tmp5:uint;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        a.position = 0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            tmp1 = a.readUnsignedInt();
            tmp2 = a.readUnsignedInt();
            tmp3 = a.readUnsignedInt();
            tmp4 = a.readUnsignedInt();
            tmp5 = a.readUnsignedInt();
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var bai: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 5000 ; i++ )
        bai.writeInt(i);
 })();
TEST(function () { bytearray_read_uint(1000, bai); }, "bytearray-read-uint-1");
