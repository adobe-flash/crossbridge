/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Probable worst case - big-endian systems (SPARC, PPC) tend to require alignment,
// so even on those systems we'll be reading byte-at-a-time.

var DESC = "Read uints from unaligned ByteArray into aligned Vector.<uint>, big-endian";
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
b.length = 4001;
b.endian = "bigEndian";

TEST(function () { for ( var i:int=0 ; i < 100 ; i++ ) { b.position = 1; extractVectorUint(b, 1000); } },
     "bytearray-to-vector-worst" );
