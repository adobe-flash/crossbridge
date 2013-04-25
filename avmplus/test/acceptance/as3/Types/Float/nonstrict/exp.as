/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the exponential function
of x (e raised to the power of x, where e is the base of the natural logarithms).
*/

// var SECTION = "4.5.22";
// var VERSION = "AS3";
// var TITLE   = "public function exp(x:float):float";


Assert.expectError("float.exp() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.exp(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.exp(string)", float.NaN, float.exp("string"));

AddStrictTestCase("float.exp(undefined)", float.NaN, float.exp(undefined));

// If x is +0, the result is 1.
AddStrictTestCase("float.exp('0')", 1f, float.exp('0'));

// If x is -0, the result is 1.
AddStrictTestCase("float.exp('-0')", 1f, float.exp('-0'));

AddStrictTestCase("float.exp(false)", 1f, float.exp(false));
AddStrictTestCase("float.exp('1')", float.E, float.exp('1'));
AddStrictTestCase("float.exp(true)", float.E, float.exp(true));


