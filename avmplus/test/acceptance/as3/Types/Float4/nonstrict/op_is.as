/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "5.5.1";
// var VERSION = "AS3";
// var TITLE   = "The is operator augmented by float4 values";


var pi_float4:Object = new float4(3.14f);
Assert.expectError("TypeError if datatype is not Class", Utils.TYPEERROR, function(){ return pi_float4 is "float4"; });

var vf = new Vector.<*>();
vf.push(float4.MAX_VALUE);
Assert.expectEq("Vector.<*> value is NOT Vector.<float4>", false, vf is Vector.<float4>);


