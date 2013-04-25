/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns the float value that is closest to x and is equal to a mathematical
integer. If two integer float values are equally close to x, then the result
is the float value that is closer to +Infinity. If x is already an integer,
the result is x.
*/

// var SECTION = "4.5.29";
// var VERSION = "AS3";
// var TITLE   = "public function round(x:float):float";


Assert.expectError("float.round() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.round(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.round(string)", float.NaN, float.round("string"));
AddStrictTestCase("float.round(undefined)", float.NaN, float.round(undefined));

// If x is +0, the result is +0.
AddStrictTestCase("float.round(false)", 0f, float.round(false));

AddStrictTestCase("float.round(true)", 1f, float.round(true));


