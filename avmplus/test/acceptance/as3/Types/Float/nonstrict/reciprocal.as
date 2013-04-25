/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "4.5.29";
// var VERSION = "AS3";
// var TITLE   = "public function reciprocal(x:float):float";


Assert.expectError("float.reciprocal() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.reciprocal(); });

// If x is NaN, the result is NaN
AddStrictTestCase("float.reciprocal(string)", float.NaN, float.reciprocal("string"));
AddStrictTestCase("float.reciprocal(undefined)", float.NaN, float.reciprocal(undefined));

// If x is +0, the result is +Infinity
AddStrictTestCase("float.reciprocal(false)", float.POSITIVE_INFINITY, float.reciprocal(false));

AddStrictTestCase("float.reciprocal(true)", float(1), float.reciprocal(true));


