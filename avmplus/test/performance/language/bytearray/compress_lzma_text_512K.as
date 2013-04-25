/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import flash.utils.CompressionAlgorithm;

var DESC = "Compress the content of 512KB ByteArray (ebook data) using LZMA compression."
include "bytearray-test-helper.as"

var a:ByteArray = prepareByteArrayWithTextDeterministically(1024 * 512);
TEST_TIME(function () { bytearray_compress(CompressionAlgorithm.LZMA, a); }, "compress_lzma_text_512K");
