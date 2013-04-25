/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.7.1";
// var VERSION = "AS3";
// var TITLE   = "The is operator augmented by float values";


function check(var1:*, var2:*):* { return var1 is var2; }

var pi_float:Object = new float(3.14);

Assert.expectEq("3.14f is float", true, pi_float is float);
Assert.expectEq("3.14f FloatLiteral is float", true, 3.14f is float);
Assert.expectEq("3.14f is NOT Number", false, pi_float is Number);
Assert.expectEq("3.14f FloatLiteral is NOT Number", false, 3.14f is Number);
Assert.expectEq("3.14f is Object", true, pi_float is Object);
Assert.expectEq("2.1f FloatLiteral is Object", true, 2.1f is Object);
Assert.expectEq("3.14f is NOT int", false, pi_float is int);
Assert.expectEq("3.14f FloatLiteral is NOT int", false, 3.14f is int);

Assert.expectEq("float.MIN_VALUE is float", true, float.MIN_VALUE is float);
Assert.expectEq("float.MAX_VALUE is float", true, float.MAX_VALUE is float);
Assert.expectEq("float.POSITIVE_INFINITY is float", true, float.POSITIVE_INFINITY is float);
Assert.expectEq("float.NEGATIVE_INFINITY is float", true, float.NEGATIVE_INFINITY is float);
Assert.expectEq("float.NaN is float", true, float.NaN is float);

var three:float = 3f;
Assert.expectEq("3f is NOT int", false, three is int);
Assert.expectEq("3f FloatLiteral is NOT int", false, 3f is int);

var minus_three:float = -3f;
Assert.expectEq("-3f is NOT int", false, minus_three is int);
Assert.expectEq("-3f FloatLiteral is NOT int", false, -3f is int);

var large_int_as_float:float = 0x7FFFFF00;
Assert.expectEq("large positive is NOT int", false, large_int_as_float is int);

var large_neg_int_as_float:float = int.MIN_VALUE;
Assert.expectEq("large negative is NOT int", false, large_neg_int_as_float is int);

Assert.expectEq("3.14f is NOT uint", false, pi_float is uint);
Assert.expectEq("3.14f FloatLiterl is NOT uint", false, 3.14f is uint);
Assert.expectEq("3f is NOT uint", false, three is uint);
Assert.expectEq("3f FloatLiteral is NOT uint", false, 3f is uint);
Assert.expectEq("-3f is NOT uint", false, minus_three is uint);
Assert.expectEq("-3f FloatLiteral is NOT uint", false, -3f is uint);

var large_uint_as_float:float = 0xFFFFFF00;
Assert.expectEq("large uint positive is NOT uint", false, large_uint_as_float is uint);
Assert.expectEq("large uint positive is NOT int", false, large_uint_as_float is int);

var zero:float = 0f;
var neg_zero:float = -0f;
Assert.expectEq("0f is NOT uint", false, zero is uint);
Assert.expectEq("0f FloatLiteral is NOT uint", false, 0f is uint);
Assert.expectEq("-0f is NOT uint", false, neg_zero is uint);
Assert.expectEq("-0f FloatLiteral is NOT uint", false, -0f is uint);

Assert.expectEq("0f is NOT String,", false, zero is String);
Assert.expectEq("0f is NOT Boolean", false, zero is Boolean);
Assert.expectEq("0f is NOT Error", false, zero is Error);
Assert.expectEq("0f FloatLiteral is NOT String,", false, 0f is String);
Assert.expectEq("0f FloatLiteral is NOT Boolean", false, 0f is Boolean);
Assert.expectEq("0f FloatLiteral is NOT Error", false, 0f is Error);

Assert.expectEq("Number.NaN is NOT float", false, Number.NaN is float);
Assert.expectEq("toplevel NaN (Number) is NOT float", false, Number.NaN is float);


Assert.expectEq("Number(float.MAX_VALUE) is NOT float", false, (new Number(float.MAX_VALUE)) is float);
Assert.expectEq("Number(float.MIN_VALUE) is NOT float", false, (new Number(float.MIN_VALUE)) is float);
Assert.expectEq("Number(Infinity) is NOT float", false, (new Number(Infinity)) is float);

AddStrictTestCase("2.01 is float", false, 2.01 is float);
AddStrictTestCase("2.01 is float check()", false, check(2.01, float));
AddStrictTestCase("3 is float", false, 3 is float);
AddStrictTestCase("3 is float check()", false, check(3, float));

Assert.expectEq("int.length (1) is NOT float", false, int.length is float);

Assert.expectEq("int.MIN_VALUE is NOT float (large negative no., but requires little mantissa)", false, int.MIN_VALUE is float);

var large_neg_int:int = int.MIN_VALUE+1;
Assert.expectEq("large_negative_int is NOT float", false, large_neg_int is float);

Assert.expectEq("uint.MAX_VALUE is NOT float (too big positive no)", false, uint.MAX_VALUE is float);

Assert.expectEq("uint.MIN_VALUE (0) is NOT float", false, uint.MIN_VALUE is float);

var large_int:int = -268435456;
Assert.expectEq("large int is NOT float (if it ends in lots of zeroes - i.e. requires less than 23bits of mantissa)", false, large_int is float);

var large_uint:uint = 268435456;
Assert.expectEq("large unsigned int is NOT float (if it ends in lots of zeroes - i.e. requires less than 23bits of mantissa)", false, large_uint is float);

var vf = new Vector.<float>();
Assert.expectEq("Vector.<float> value is Vector.<float>", true, vf is Vector.<float>);
Assert.expectEq("Vector.<float> value is Object", true, vf is Object);
Assert.expectEq("Vector.<float> value is NOT Vector.<Number>", false, vf is Vector.<Number>);
Assert.expectEq("Vector.<float> value is NOT Vector.<Object>", false, vf is Vector.<Object>);
Assert.expectEq("Vector.<float> value is NOT Vector.<*>", false, vf is Vector.<*>);
vf = new Vector.<*>();
vf.push(float.MAX_VALUE);
Assert.expectEq("Vector.<*> value is NOT Vector.<float>", false, vf is Vector.<float>);

vf = new Vector.<Number>();
Assert.expectEq("Vector.<Number> value is NOT Vector.<float>", false, vf is Vector.<float>);
vf = new Vector.<String>();
Assert.expectEq("Vector.<String> value is NOT Vector.<float>", false, vf is Vector.<float>);
vf = new Vector.<Object>();
Assert.expectEq("Vector.<Object> value is NOT Vector.<float>", false, vf is Vector.<float>);
vf = new Vector.<int>();
Assert.expectEq("Vector.<int> value is NOT Vector.<float>", false, vf is Vector.<float>);
vf = new Vector.<uint>();
Assert.expectEq("Vector.<uint> value is NOT Vector.<float>", false, vf is Vector.<float>);

Assert.expectEq("String is NOT float", false, "twelve" is float);
var myObject:Object = {};
Assert.expectEq("Object is NOT float", false, myObject is float);
Assert.expectEq("Boolean is NOT float", false, true is float);
var myArray:Array = new Array();
Assert.expectEq("Array is NOT float", false, myArray is float);



