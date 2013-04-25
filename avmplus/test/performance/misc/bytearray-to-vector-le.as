/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read uints from aligned ByteArray into Vector.<uint>, little-endian";
include "driver.as";

import flash.utils.ByteArray;

// Typical use case: read a Vector.<uint> from a ByteArray, starting
// at bytearray's position and using the endianness set by the caller,
// extracting "len" values.

function extractVectorUint(b: ByteArray, len: int): Vector.<uint>
{
    var v: Vector.<uint> = new Vector.<uint>(len);
    for ( var j:int = 0 ; j < len ; j++ )
        v[j] = b.readUnsignedInt();
    return v;
}

var b: ByteArray = new ByteArray;
b.length = 4000;
b.endian = "littleEndian";

TEST(function () { for ( var i:int=0 ; i < 100 ; i++ ) { b.position = 0; extractVectorUint(b, 1000); } },
     "bytearray-to-vector-le" );
