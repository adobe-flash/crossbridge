/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "5.5.2";
// var VERSION = "AS3";
// var TITLE   = "Vector.<float4> access beyond vector length";


var vec:Vector.<float4> = new <float4> [float4(0, 0, 0, 0),
                                        float4(1, 1, 1, 1),
                                        float4(2, 2, 2, 2)];

for (var j:int = 0; j < vec.length; j++)
    AddStrictTestCase("Global: read", float4(j, j, j, j), vec[j]);

Assert.expectError("Global: vector access beyond length", Utils.RANGEERROR+1125,  function(){ vec[vec.length+1]; });
Assert.expectError("Global: vector access -1", Utils.RANGEERROR+1125,  function(){ vec[-1]; });


function getValue(flt4:Vector.<float4>, index:int):float4 { return flt4[index]; }
for (var j:int = 0; j < vec.length; j++)
    AddStrictTestCase("Function: read", float4(j, j, j, j), getValue(vec, j));

Assert.expectError("Function: vector access beyond length", Utils.RANGEERROR+1125,  function(){ getValue(vec, vec.length+1); });
Assert.expectError("Function: vector access -1", Utils.RANGEERROR+1125,  function(){ getValue(vec, -1); });



