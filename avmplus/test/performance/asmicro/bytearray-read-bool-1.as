/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read bool from ByteArray, store in local with type bool.";
include "driver.as"

function bytearray_read_bool(iter: int, a: ByteArray): Boolean
{
    var tmp1:Boolean;
    var tmp2:Boolean;
    var tmp3:Boolean;
    var tmp4:Boolean;
    var tmp5:Boolean;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        a.position = 0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            tmp1 = a.readBoolean();
            tmp2 = a.readBoolean();
            tmp3 = a.readBoolean();
            tmp4 = a.readBoolean();
            tmp5 = a.readBoolean();
        }
    }
    return tmp1 && tmp2 && tmp3 && tmp4 && tmp5;
}

var bai: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 5000 ; i++ )
        bai.writeBoolean((i & 1));
 })();
TEST(function () { bytearray_read_bool(1000, bai); }, "bytearray-read-bool-1");
