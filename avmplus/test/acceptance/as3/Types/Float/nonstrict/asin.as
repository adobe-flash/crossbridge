/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the arc sine of x.
The result is expressed in radians and ranges from -PI/2 to +PI/2.
*/

// var SECTION = "4.5.17";
// var VERSION = "AS3";
// var TITLE   = "public function asin(x:float):float";


Assert.expectError("float.asin() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.asin(); });

AddStrictTestCase("float.asin(undefined)", float.NaN, float.asin(undefined));
AddStrictTestCase("float.asin(true)", float.PI/2f, float.asin(true));
AddStrictTestCase("float.asin(false)", 0f, float.asin(false));

AddStrictTestCase("float.asin('1')", float.PI/2f, float.asin('1'));
AddStrictTestCase("float.asin('0')", 0f, float.asin('0'));


