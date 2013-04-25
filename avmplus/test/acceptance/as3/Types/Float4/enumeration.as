/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.6.5.1";
// var VERSION = "AS3";
// var TITLE   = "Enumeration for..in";


var expected:* = 4;
var result:* = 0;
var flt4:float4 = new float4(1f, 1f, 1f, 1f);
for ( var i in flt4 )
{
    Assert.expectEq("for .. in loop iterant is index NOT value", i, result);
    result++;
}
Assert.expectEq("for .. in loop over float4 should iterate 4 times", expected, result);


// every
var float4Array:Array = [float4(0f), float4(1f), float4(2f), float4(3f)];
var resultVal:float4 = new float4(0f);
function everyFunc(val, index, obj)
{
    Assert.expectEq("every val is passed as float4", "float4", getQualifiedClassName(val));
    resultVal+=val;
    return true;
}
float4Array.every(everyFunc);
Assert.expectEq("float4Array.every() sum up", new float4(6f), resultVal);


// forEach
float4Array = [float4(0f), float4(1f), float4(2f), float4(3f)];
resultVal = new float4(0f);
function forEachFunc(val, index, obj)
{
    Assert.expectEq("forEach val is passed as float4", "float4", getQualifiedClassName(val));
    resultVal+=val;
}
float4Array.forEach(forEachFunc);
Assert.expectEq("float4Array.forEach() sum up", new float4(6f), resultVal);


// map
float4Array = [float4(0f), float4(1f), float4(2f), float4(3f)];
resultVal = new float4(0f);
function mapFunc(val, index, obj)
{
    Assert.expectEq("map val is passed as float4", "float4", getQualifiedClassName(val));
    return val;
}
Assert.expectEq("float4Array.map()", [float4(0f), float4(1f), float4(2f), float4(3f)].toString(), float4Array.map(mapFunc).toString());
Assert.expectEq("float4Array.map() returns a new array", true, float4Array.map(mapFunc) !== float4Array);


// forEach
float4Array = [float4(0f), float4(1f), float4(2f), float4(3f)];
resultVal = new float4(0f);
function someFunc(val, index, obj)
{
    Assert.expectEq("some val is passed as float4", "float4", getQualifiedClassName(val));
    resultVal+=val;
}
float4Array.some(someFunc);
Assert.expectEq("float4Array.some() sum up", new float4(6f), resultVal);


// join
float4Array = [float4(0f), float4(1f), float4(2f), float4(3f)];
Assert.expectEq("float4Array.join()", "0,0,0,0|1,1,1,1|2,2,2,2|3,3,3,3", float4Array.join("|"));

var float4Array:Array = [float4(0f), float4(1f), float4(2f), float4(2f), float4(3f)];
Assert.expectEq("float4Array.indexOf(float4(0f, 1f, 2f, 3f))", -1, float4Array.indexOf(float4(0f, 1f, 2f, 3f)));
Assert.expectEq("float4Array.indexOf(float4(2f))", 2, float4Array.indexOf(float4(2f)));
Assert.expectEq("float4Array.lastIndexOf(float4(0f, 1f, 2f, 3f))", -1, float4Array.lastIndexOf(float4(0f, 1f, 2f, 3f)));
Assert.expectEq("float4Array.lastIndexOf(float4(2f))", 3, float4Array.lastIndexOf(float4(2f)));



