/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the result of raising x to the power y.
*/

// var SECTION = "4.5.27";
// var VERSION = "AS3";
// var TITLE   = "public function pow(x:float,y:float):float";


Assert.expectError("float.pow() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.pow(); });
Assert.expectError("float.pow() with one args", Utils.ARGUMENTERROR+1063,  function(){ float.pow(1); });

// If y is NaN, the result is NaN.
AddStrictTestCase("float.pow(1.0f, string)", float.NaN, float.pow(1.0f, "string"));
AddStrictTestCase("float.pow(1.0f, undefined)", float.NaN, float.pow(1.0f, undefined));

// If y is +0, the result is 1, even if x is NaN.
AddStrictTestCase("float.pow(string, 0f)", 1f, float.pow("string", 0f));
AddStrictTestCase("float.pow(undefined, 0f)", 1f, float.pow(undefined, 0f));

// If y is -0, the result is 1, even if x is NaN.
AddStrictTestCase("float.pow(string, -0f)", 1f, float.pow("string", -0f));
AddStrictTestCase("float.pow(undefined, -0f)", 1f, float.pow(undefined, -0f));

// If x is NaN and y is nonzero, the result is NaN.
AddStrictTestCase("float.pow(string, 1f)", float.NaN, float.pow("string", 1f));
AddStrictTestCase("float.pow(undefined, 1f)", float.NaN, float.pow(undefined, 1f));


