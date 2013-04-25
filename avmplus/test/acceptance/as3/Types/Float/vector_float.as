/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "13";
// var VERSION = "AS3";
// var TITLE   = "Vector.<float>";


var f1:float = 3.1415f;
var f2:float = float.MAX_VALUE;
var f3:float = -0.00032f;
var f4:float = -12.375f;

var vecFlt:Vector.<float> = new Vector.<float>();
vecFlt.push(f1);
vecFlt.push(f2);
vecFlt.push(f3);
vecFlt.push(f4);

Assert.expectEq("vecflt[0] value", f1, vecFlt[0]);
Assert.expectEq("vecflt[0] typeof", "float", typeof vecFlt[0]);
Assert.expectEq("vecflt[1] value", f2, vecFlt[1]);
Assert.expectEq("vecflt[1] typeof", "float", typeof vecFlt[1]);
Assert.expectEq("vecflt[2] value", f3, vecFlt[2]);
Assert.expectEq("vecflt[2] typeof", "float", typeof vecFlt[2]);
Assert.expectEq("vecflt[3] value", f4, vecFlt[3]);
Assert.expectEq("vecflt[3] typeof", "float", typeof vecFlt[3]);


// Bug 697741 - Vector optimizations for float are miss using type information
var v:Vector.<float> = new <float> [1,1,1,1];
AddStrictTestCase("v[0] value", 1f, v[0]);
foo(v);
function foo(vec:Vector.<float>){ AddStrictTestCase("v[0] value", 1f, vec[0]); }


