/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import flash.utils.CompressionAlgorithm;
import avmplus.File;

var DESC = "Decompress a 1K ByteArray with LZMA."
include "bytearray-test-helper.as"

var data : ByteArray = prepareByteArrayDeterministically(1024);
data.compress(CompressionAlgorithm.LZMA);

TEST_TIME(function () { data.uncompress(CompressionAlgorithm.LZMA); }, "uncompress_lzma_bin_1K");
