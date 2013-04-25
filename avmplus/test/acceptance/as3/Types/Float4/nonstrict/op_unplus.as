/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "5.1.2";
// var VERSION = "AS3";
// var TITLE   = "The numeric identity operator +";


// https://bugzilla.mozilla.org/show_bug.cgi?id=698367
// Unary operators do not handle valueOf conversion returning non-Number on
// objects of known type
class C {}
C.prototype.valueOf = function () { return float4(1,1,1,1); };
var c:C = new C;  // It's important that the :C is here
AddStrictTestCase("unary plus via valueOf simple f4", float4(1,1,1,1), +c);

class D {}
D.prototype.valueOf = function () { return float4(1,-1,2,-3); };
var d:D = new D;  // It's important that the :D is here
AddStrictTestCase("unary plus via valueOf", float4(1,-1,2,-3), +d);


