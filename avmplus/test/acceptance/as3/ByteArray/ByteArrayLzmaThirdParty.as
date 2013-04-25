/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// General principles for this test suite:
//
//  - never write just one, write at least two (to test that position
//    advances correctly and output is placed correctly)
//  - ditto read
//  - test both little and big endian for multibyte data
//  - test both aligned and unaligned access for multibyte data
//
// Search for "TODO" for comments about missing tests.

import flash.errors.EOFError;
import flash.errors.IOError;
import flash.utils.ByteArray;
import flash.utils.CompressionAlgorithm;

import avmplus.File;
import com.adobe.test.Assert;

// var SECTION = "ByteArrayWithLzmaThirdParty";
// var VERSION = "as3";
// var TITLE   = "test ByteArray class with lzma inputs generated via LZMA.jar";


// Bugzilla 733051: ByteArrayLzma callCompress tests are doing
// round-trips through compress and uncompress; this test is checking
// that we correctly handle inputs that have been compressed by
// third-party utilities (see LZMA.jar attached to Bugzilla 729336).
function testThirdPartyLzmaInputs()
{
    var compressedHello:ByteArray = File.readByteArray("as3/ByteArray/ByteArrayLzmaThirdParty.abc-compressed-hello.lz");
    var compressedSmall:ByteArray = File.readByteArray("as3/ByteArray/ByteArrayLzmaThirdParty.abc-compressed-small.lz");

    var uncompressedHello:ByteArray = File.readByteArray("as3/ByteArray/ByteArrayLzmaThirdParty.abc-uncompressed-hello.txt");
    var uncompressedSmall:ByteArray = File.readByteArray("as3/ByteArray/ByteArrayLzmaThirdParty.abc-uncompressed-small.txt");

    var helloString:String = uncompressedHello.readUTFBytes(uncompressedHello.length);
    var smallString:String = uncompressedSmall.readUTFBytes(uncompressedSmall.length);

    compressedHello.uncompress(CompressionAlgorithm.LZMA);
    var helloString2:String = compressedHello.readUTFBytes(compressedHello.length);
    Assert.expectEq("Correct lzma uncompression on hello", helloString, helloString2);

    compressedSmall.uncompress(CompressionAlgorithm.LZMA);
    var smallString2:String = compressedSmall.readUTFBytes(compressedSmall.length);
    Assert.expectEq("Correct lzma uncompression on small", smallString, smallString2);
}

testThirdPartyLzmaInputs();

