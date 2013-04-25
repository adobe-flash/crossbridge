/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
Returns an implementation-dependent approximation to the arc tangent of x.
The result is expressed in radians and ranges from iPI/2 to +PI/2.
*/

// var SECTION = "XXX";
// var VERSION = "AS3";
// var TITLE   = "JSON functionality operating on float4";


// Testing the following on float4:
//
//  - float4 data are formatted as objects with x, y, z, w fields
//  - structure is walked properly if replacer or property array is present
//  - toJSON on float4.prototype is obeyed
//  - toJSON on float.prototype [sic] is obeyed

var x = JSON.stringify(float4(1,2,3,4));
Assert.expectEq("float4 serializes as structure of numbers", x, x.match(/^\{"[xyzw]":[1234],"[xyzw]":[1234],"[xyzw]":[1234],"[xyzw]":[1234]\}$/)[0]);
Assert.expectEq("All fields present", true, x.match(/x/) != null && x.match(/y/) != null && x.match(/z/) != null && x.match(/w/) != null);

float4.prototype.toJSON = function (x) { return "hi there" };
Assert.expectEq("float4.prototype.toJSON is obeyed", "\"hi there\"", JSON.stringify(float4(1,2,3,4)));
delete float4.prototype.toJSON;

float.prototype.toJSON = function (k) { return this + 5 };
var x = JSON.stringify(float4(1,2,3,4));
Assert.expectEq("float4 serializes as structure of larger numbers", x, x.match(/^\{"[xyzw]":[6789],"[xyzw]":[6789],"[xyzw]":[6789],"[xyzw]":[6789]\}$/)[0]);
delete float.prototype.toJSON;

var x = JSON.stringify(float4(1,2,3,4), function (k,v) { if (v is float) return v+5; return v; }, 1);
Assert.expectEq("float4 serializes as structure of larger numbers with indent", x, x.match(/^\{\n "[xyzw]": [6789],\n "[xyzw]": [6789],\n "[xyzw]": [6789],\n "[xyzw]": [6789]\n\}$/)[0]);

