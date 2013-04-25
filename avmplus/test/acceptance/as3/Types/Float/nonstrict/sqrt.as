/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the square root of x.
*/

// var SECTION = "4.5.31";
// var VERSION = "AS3";
// var TITLE   = "public function sqrt(x:float):float";


Assert.expectError("float.sqrt() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.sqrt(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.sqrt(string)", float.NaN, float.sqrt("string"));
AddStrictTestCase("float.sqrt(undefined)", float.NaN, float.sqrt(undefined));

// If x is +0, the result is +0.
AddStrictTestCase("float.sqrt(false)", 0f, float.sqrt(false));

AddStrictTestCase("float.sqrt(true)", 1f, float.sqrt(true));


