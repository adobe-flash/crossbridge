/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Store uints from Vector.<uint> into aligned ByteArray, little-endian";
include "driver.as";

import flash.utils.ByteArray;

// Typical use case: store a Vector.<uint> into a ByteArray, starting
// at bytearray's position and using the endianness set by the caller,
// extracting "len" values.

function injectVectorUint(b: ByteArray, v: Vector.<uint>, len: int): Vector.<uint>
{
    for ( var j:int = 0 ; j < len ; j++ )
        b.writeUnsignedInt(v[j]);
    return v;
}

var b: ByteArray = new ByteArray;
b.length = 4000;
b.endian = "littleEndian";
var v: Vector.<uint> = new Vector.<uint>(1000);

TEST(function () { for ( var i:int=0 ; i < 100 ; i++ ) { b.position = 0; injectVectorUint(b, v, 1000); } },
     "vector-to-bytearray-le" );
