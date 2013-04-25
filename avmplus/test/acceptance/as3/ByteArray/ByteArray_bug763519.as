/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import avmplus.System;
import com.adobe.test.Assert;

// var SECTION = "ByteArray";
// var VERSION = "as3";
// var TITLE   = "test ByteArray capacity growth regression";


var b:ByteArray = new ByteArray();
var MB:uint = 1024*1024;

// Initial State: Measure private mem before any real byte array allocation.
var privMem_0:uint = System.privateMemory;

// Test setup: Start off with 100 MB byte array.
b.length = 100*MB;
b[100*MB - 1] = 1;

var privMem_1:uint = System.privateMemory;

// First test: Increase byte array by 30 MB.
// Capacity should *not* increase to 200 MB.
b.length = 130*MB;

var privMem_2:uint = System.privateMemory;

var delta_0_1:int = (privMem_1 - privMem_0);
var delta_0_2:int = (privMem_2 - privMem_0);

var deltaLessThan2x:Boolean = (delta_0_2 < 2 * delta_0_1);

// Second test: Cut byte array to 1 MB.
// Capacity should be cut down from the original 100 MB.
b.length = 1*MB;

var privMem_3:uint = System.privateMemory;
var delta_1_3:int = (privMem_3 - privMem_1);
var deltaNegative:Boolean = (delta_1_3 < 0);


var info = {};
info.privMem_0 = privMem_0;
info.privMem_1 = privMem_1;
info.privMem_2 = privMem_2;
info.privMem_3 = privMem_3;
info.delta_0_1 = delta_0_1;
info.delta_0_2 = delta_0_2;
info.delta_1_3 = delta_1_3;
info.deltaLessThan2x = deltaLessThan2x;
info.deltaNegative = deltaNegative;

print(JSON.stringify(info));

Assert.expectEq("small length increase incurs less than 2x capacity growth",
            true, deltaLessThan2x);

Assert.expectEq("huge length decrease causes capacity drop",
            true, deltaNegative);

