/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns the smallest (closest to -Infinity) float value that is not less than x
and is equal to a mathematical integer. If x is already an integer, the result is x.
*/

// var SECTION = "4.5.20";
// var VERSION = "AS3";
// var TITLE   = "public function ceil(x:float):float";


Assert.expectError("float.ceil() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.ceil(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.ceil(string)", float.NaN, float.ceil("string"));

AddStrictTestCase("float.ceil(undefined)", float.NaN, float.ceil(undefined));

AddStrictTestCase("float.ceil(true)", 1f, float.ceil(true));
AddStrictTestCase("float.ceil(false)", 0f, float.ceil(false));

AddStrictTestCase("float.ceil('1')", 1f, float.ceil('1'));
AddStrictTestCase("float.ceil('0')", 0f, float.ceil('0'));



