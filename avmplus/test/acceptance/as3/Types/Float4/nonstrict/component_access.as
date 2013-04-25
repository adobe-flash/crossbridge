/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "4.6.3";
// var VERSION = "AS3";
// var TITLE   = "Component accesses";


// Access component via v.x
var flt4:float4 = new float4(1f);
Assert.expectError("float4.x as a setter", Utils.REFERENCEERROR, function(){ flt4.x = 12f });
Assert.expectError("float4.y as a setter", Utils.REFERENCEERROR, function(){ flt4.y = 12f });
Assert.expectError("float4.z as a setter", Utils.REFERENCEERROR, function(){ flt4.z = 12f });
Assert.expectError("float4.w as a setter", Utils.REFERENCEERROR, function(){ flt4.w = 12f });

// Access outside of valid index ranges section 4.6.3.2.b
Assert.expectError("float4[-1]", Utils.REFERENCEERROR, function(){ flt4[-1]; });
Assert.expectError("float4[4]", Utils.RANGEERROR, function(){ flt4[4]; });


