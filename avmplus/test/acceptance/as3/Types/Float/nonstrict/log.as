/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the natural logarithm of x.
*/

// var SECTION = "4.5.24";
// var VERSION = "AS3";
// var TITLE   = "public function log(x:float):float";


Assert.expectError("float.log() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.log(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.log(string)", float.NaN, float.log("string"));
AddStrictTestCase("float.log(undefined)", float.NaN, float.log(undefined));

// If x is +0 or -0, the result is -Infinity.
AddStrictTestCase("float.log(false)", float.NEGATIVE_INFINITY, float.log(false));

// If x is 1, the result is +0.
AddStrictTestCase("float.log(true)", float(0), float.log(true));


