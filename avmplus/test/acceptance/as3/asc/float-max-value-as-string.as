/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 709760

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Constant-folding of String(float.MAX_VALUE)";


var x = String(float.MAX_VALUE);
var y = String(Number.MAX_VALUE);
var z = String(float4(float.MAX_VALUE))

//print(x);
//print(y);

Assert.expectEq("Looks like a scientific format number: float", true, x.match(/^\d\.\d+e\+\d+$/) != null);
Assert.expectEq("Looks like a scientific format number: Number", true, y.match(/^\d\.\d+e\+\d+$/) != null);
Assert.expectEq("Looks like four scientific format numbers: float4", true, z.match(/^\d\.\d+e\+\d+,\d\.\d+e\+\d+,\d\.\d+e\+\d+,\d\.\d+e\+\d+$/) != null);

