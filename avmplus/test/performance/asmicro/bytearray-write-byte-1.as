/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Write byte to ByteArray, from literal data.  Compare bytearray-write-byte-{2,3}.";
include "driver.as"

function bytearray_write_byte(iter: int, a: ByteArray): void
{
    for ( var i:int = 0 ; i < iter ; i++ ) {
        a.position = 0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            a.writeByte(1);
            a.writeByte(2);
            a.writeByte(3);
            a.writeByte(4);
            a.writeByte(5);
        }
    }
}

var bai: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 5000 ; i++ )
        bai.writeByte(i);
 })();
TEST(function () { bytearray_write_byte(1000, bai); }, "bytearray-write-byte-1");
