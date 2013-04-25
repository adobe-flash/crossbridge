/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=561249
 *
 */
//-----------------------------------------------------------------------------

import avmplus.System;
import com.adobe.test.Assert;

// var SECTION = "561249";
// var VERSION = "";
// var TITLE   = "Specialized Addition Helper Functions";
// var bug = "561249";


var array:Array = new Array();
var item:int = 0;

function VerifyEquals(actual, expect, a, b, ctx)
{
    var status = ctx + " " + a + " + " + b;
    array[item++] = Assert.expectEq( status, expect, actual);
}

function canon(a)
{
    return System.canonicalizeNumber(a);
}

function AddIntToAtom(a:int, b, c)
{
    VerifyEquals(a + canon(b), c, a, b, "AddIntToAtom");
}

function AddAtomToInt(a, b:int, c)
{
    VerifyEquals(canon(a) + b, c, a, b, "AddAtomToInt");
}

function AddDoubleToAtom(a:Number, b, c)
{
    VerifyEquals(a + canon(b), c, a, b, "AddDoubleToAtom");
}

function AddAtomToDouble(a, b:Number, c)
{
    VerifyEquals(canon(a) + b, c, a, b, "AddAtomToDouble");
}

function AddAtomToAtom(a, b, c)
{
    VerifyEquals(canon(a) + canon(b), c, a, b, "AddAtomToAtom");
}

var z = null;
var z_n:Number = z;

var n = new Namespace("http://www.example.com/");
var n_n:Number = n;

var s = "foo";
var s_s:String = s;

var o = new Object();
var o_s:String = o;

var d = new Date(0);
var d_s:String = d;

var x = new XML("<a><b/></a>");
var x_s:String = x;

var l = new XMLList("<a>one</a><b>two</b>");
var l_s:String = l;

var int32_max = 2147483647;

// We test cases in which a numeric value is added/concatenated to a non-numeric atom.
// We characterize the value of the non-numeric atom as to whether it results in a numeric
// addition or a string concatentation, i.e., behaves like a number or a string, when
// combined with a number.

function LikeNumber(i, a, a_n:Number)
{
    var i_n:Number = i;

    //NOTE: We assume that typed Number + Number addition functions correctly.

    if (i <= int32_max) {
        AddIntToAtom (i, a, i_n + a_n);
        AddAtomToInt (a, i, a_n + i_n);
    }
    AddDoubleToAtom  (i, a, i_n + a_n);
    AddAtomToDouble  (a, i, a_n + i_n);
    AddAtomToAtom    (i, a, i_n + a_n);
    AddAtomToAtom    (a, i, a_n + i_n);
}

function LikeString(i, a, a_s:String)
{
    var i_s:String = i;

    //NOTE: We assume that typed String + String concatenation functions correctly.

    if (i <= int32_max) {
        AddIntToAtom (i, a, i_s + a_s);
        AddAtomToInt (a, i, a_s + i_s);
    }
    AddDoubleToAtom  (i, a, i_s + a_s);
    AddAtomToDouble  (a, i, a_s + i_s);
    AddAtomToAtom    (i, a, i_s + a_s);
    AddAtomToAtom    (a, i, a_s + i_s);
}

function TryIntValue(i)
{
    LikeNumber(i, z, z_n);  // null
    LikeNumber(i, n, n_n);  // Namespace

    LikeString(i, s, s_s);  // String
    LikeString(i, o, o_s);  // Object
    LikeString(i, d, d_s);  // Date
    LikeString(i, x, x_s);  // XML
    LikeString(i, l, l_s);  // XMLList
}

var i = 555;               // Fits in 29-bit intptr
var j = 555555555;         // Won't fit in 29-bit intptr
var k = 5555555555555555;  // Won't fit in 53-bit intptr

function getTestCases()
{
   TryIntValue(i);
   TryIntValue(j);
   TryIntValue(k);

   return array;
}

var testcases = getTestCases();
