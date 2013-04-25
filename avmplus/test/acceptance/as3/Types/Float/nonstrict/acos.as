/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the arc cosine of x.
The result is expressed in radians and ranges from +0 to +PI.
*/

// var SECTION = "4.5.16";
// var VERSION = "AS3";
// var TITLE   = "public function acos(x:float):float";


Assert.expectError("float.acos() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.acos(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.acos(string)", float.NaN, float.acos("string"));

AddStrictTestCase("float.acos(undefined)", float.NaN, float.acos(undefined));

AddStrictTestCase("float.acos(true)", 0f, float.acos(true));
AddStrictTestCase("float.acos(false)", float.PI/2.0f, float.acos(false));

AddStrictTestCase("float.acos('1')", 0f, float.acos('1'));
AddStrictTestCase("float.acos('0')", float.PI/2.0f, float.acos('0'));


