/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns the absolute value of x; the result has the same magnitude as x but has positive sign.
*/

// var SECTION = "4.5.15";
// var VERSION = "AS3";
// var TITLE   = "public function abs(x:float):float";


Assert.expectError("float.abs() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.abs(); });

AddStrictTestCase("float.abs(undefined)", float.NaN, float.abs(undefined));
AddStrictTestCase("float.abs(true)", 1f, float.abs(true));
AddStrictTestCase("float.abs(false)", 0f, float.abs(false));

AddStrictTestCase("float.abs('1')", 1f, float.abs('1'));
AddStrictTestCase("float.abs('0')", 0f, float.abs('0'));
AddStrictTestCase("float.NEGATIVE_INFINITY/float.abs('0')", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.abs('0'));

