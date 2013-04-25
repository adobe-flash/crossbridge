/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read aligned bigEndian double from ByteArray, store in local with type Number.";
include "driver.as"

function bytearray_read_double(iter: int, a: ByteArray): Number
{
    var tmp1:Number;
    var tmp2:Number;
    var tmp3:Number;
    var tmp4:Number;
    var tmp5:Number;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        a.position = 0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            tmp1 = a.readDouble();
            tmp2 = a.readDouble();
            tmp3 = a.readDouble();
            tmp4 = a.readDouble();
            tmp5 = a.readDouble();
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var bai: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 5000 ; i++ )
        bai.writeDouble(i);
 })();
TEST(function () { bytearray_read_double(1000, bai); }, "bytearray-read-double-1");
