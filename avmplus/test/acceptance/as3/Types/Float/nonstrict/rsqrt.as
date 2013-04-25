/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns reciprocal square root of scalars.
Testmedia based on edge points in float.sqrt().
*/

// var SECTION = "4.5.31";
// var VERSION = "AS3";
// var TITLE   = "public function rsqrt(x:float):float";


Assert.expectError("float.rsqrt() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.rsqrt(); });

// If x is NaN, the result is NaN. (1/NaN == NaN)
AddStrictTestCase("float.rsqrt(string)", float.NaN, float.rsqrt("string"));
AddStrictTestCase("float.rsqrt(undefined)", float.NaN, float.rsqrt(undefined));

// If x is +0, the result is +0. (1/0 == Infinity)
AddStrictTestCase("float.rsqrt(false)", float.POSITIVE_INFINITY, float.rsqrt(false));

AddStrictTestCase("float.rsqrt(true)", float(1), float.rsqrt(true));


