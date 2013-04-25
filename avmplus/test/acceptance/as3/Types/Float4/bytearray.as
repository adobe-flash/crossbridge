/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "XX";
// var VERSION = "AS3";
// var TITLE   = "ByteArray readFloat4() writeFloat4";



var f4:float4 = new float4(12.375f, 2.125f, 1.0f, 3.125f);
var ba_test_Obj:ByteArray = new ByteArray();
ba_test_Obj.writeFloat4(f4);
ba_test_Obj.position = 0;

AddStrictTestCase("write/read float4 in a ByteArray",  float4(12.375f, 2.125f, 1.0f, 3.125f), ba_test_Obj.readFloat4());


