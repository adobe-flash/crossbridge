/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns the greatest (closest to +Infinity) float value that is not greater
than x and is equal to a mathematical integer. If x is already an integer, the
result is x.
*/

// var SECTION = "4.5.23";
// var VERSION = "AS3";
// var TITLE   = "public function floor(x:float):float";


Assert.expectError("float.floor() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.floor(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.floor(string)", float.NaN, float.floor("string"));
AddStrictTestCase("float.floor(undefined)", float.NaN, float.floor(undefined));

// If x is +0, the result is +0.
AddStrictTestCase("float.floor(false)", 0f, float.floor(false));
AddStrictTestCase("float.floor('0')", 0f, float.floor('0'));
AddStrictTestCase("float.INFINITY/float.floor('0')", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.floor('0'));


AddStrictTestCase("float.floor(true)", 1f, float.floor(true));
AddStrictTestCase("float.floor('1')", 1f, float.floor('1'));



