/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.5.1";
// var VERSION = "AS3";
// var TITLE   = "The is operator augmented by float4 values";


var pi_float4:Object = new float4(3.14f);
var pi_float:Object = new float(3.14f);

Assert.expectEq("float4(3.14f) is float", true, pi_float4 is float4);
Assert.expectEq("float4(3.14f) is NOT Number", false, pi_float4 is Number);
Assert.expectEq("float4(3.14f) is Object", true, pi_float4 is Object);
Assert.expectEq("float4(3.14f) is NOT int", false, pi_float4 is int);

Assert.expectEq("float(3.14f) is NOT float4", false, float(3.14f) is float4);
Assert.expectEq("float var is NOT float4", false, pi_float is float4);

var three:float4 = float4(3f);
Assert.expectEq("float4(3f) is NOT int", false, three is int);

var minus_three:float4 = float4(-3f);
Assert.expectEq("float4(-3f) is NOT int", false, minus_three is int);
Assert.expectEq("float4(3.14f) is NOT uint", false, pi_float4 is uint);
var zero:float4 = float4(0f);
var neg_zero:float4 = float4(-0f);
Assert.expectEq("float4(0f) is NOT uint", false, zero is uint);
Assert.expectEq("float4(-0f) is NOT uint", false, neg_zero is uint);

Assert.expectEq("float4(0f) is NOT String,", false, zero is String);
Assert.expectEq("float4(0f) is NOT Boolean", false, zero is Boolean);
Assert.expectEq("float4(0f) is NOT Error", false, zero is Error);
Assert.expectEq("Number.NaN is NOT float4", false, Number.NaN is float4);


Assert.expectEq("Number(Infinity) is NOT float4", false, (new Number(Infinity)) is float4);

var vf = new Vector.<float4>();
Assert.expectEq("Vector.<float4> value is Vector.<float4>", true, vf is Vector.<float4>);
Assert.expectEq("Vector.<float4> value is Object", true, vf is Object);
Assert.expectEq("Vector.<float4> value is NOT Vector.<Number>", false, vf is Vector.<Number>);
Assert.expectEq("Vector.<float4> value is NOT Vector.<Object>", false, vf is Vector.<Object>);
Assert.expectEq("Vector.<float4> value is NOT Vector.<*>", false, vf is Vector.<*>);

vf = new Vector.<Number>();
Assert.expectEq("Vector.<Number> value is NOT Vector.<float4>", false, vf is Vector.<float4>);
vf = new Vector.<String>();
Assert.expectEq("Vector.<String> value is NOT Vector.<float4>", false, vf is Vector.<float4>);
vf = new Vector.<Object>();
Assert.expectEq("Vector.<Object> value is NOT Vector.<float4>", false, vf is Vector.<float4>);
vf = new Vector.<int>();
Assert.expectEq("Vector.<int> value is NOT Vector.<float4>", false, vf is Vector.<float4>);
vf = new Vector.<uint>();
Assert.expectEq("Vector.<uint> value is NOT Vector.<float4>", false, vf is Vector.<float4>);

Assert.expectEq("String is NOT float4", false, "twelve" is float4);
var myObject:Object = {};
Assert.expectEq("Object is NOT float4", false, myObject is float4);
Assert.expectEq("Boolean is NOT float4", false, true is float4);
var myArray:Array = new Array();
Assert.expectEq("Array is NOT float4", false, myArray is float4);



