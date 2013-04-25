/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read byte from ByteArray with [] operator and int index, store in local with type int.  Compare bytearray-read-byte-{1,3}.";
include "driver.as"

function bytearray_read_byte(iter: int, a: ByteArray): int
{
    var tmp1:int;
    var tmp2:int;
    var tmp3:int;
    var tmp4:int;
    var tmp5:int;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        // Please don't move the increment of j into the brackets in the code below
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp1 = a[j]; j++;
            tmp2 = a[j]; j++;
            tmp3 = a[j]; j++;
            tmp4 = a[j]; j++;
            tmp5 = a[j]; j++;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var bai: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 5000 ; i++ )
        bai.writeByte(i);
 })();
TEST(function () { bytearray_read_byte(1000, bai); }, "bytearray-read-byte-2");
