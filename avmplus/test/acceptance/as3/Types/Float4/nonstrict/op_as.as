/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "5.5.2";
// var VERSION = "AS3";
// var TITLE   = "The as operator augmented by float4 values";


var pi_float4:Object = new float4(3.14f);
var large_uint_as_float4:float4 = 0xFFFFFF00;
Assert.expectError("AS: TypeError if datatype is not Class", Utils.TYPEERROR, function(){ return pi_float4 as "float4"; });


var vf = new Vector.<float4>();
vf = new Vector.<*>();
vf.push(float4.MAX_VALUE);
Assert.expectEq("Vector.<*> value as Vector.<float4>", null, vf as Vector.<float4>);


