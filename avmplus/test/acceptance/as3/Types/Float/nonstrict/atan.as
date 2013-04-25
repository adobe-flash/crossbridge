/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the arc tangent of x.
The result is expressed in radians and ranges from iPI/2 to +PI/2.
*/

// var SECTION = "4.5.18";
// var VERSION = "AS3";
// var TITLE   = "public function atan(x:float):float";


function check(param:float):float { return float.atan(param); }

AddStrictTestCase("float.atan() returns a float", "float", getQualifiedClassName(float.atan(0)));
AddStrictTestCase("float.atan() length is 1", 1, float.atan.length);
Assert.expectError("float.atan() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.atan(); });

// If x is NaN, the result is NaN.
AddStrictTestCase("float.atan(string)", float.NaN, float.atan("string"));
AddStrictTestCase("float.atan(undefined)", float.NaN, float.atan(undefined));
AddStrictTestCase("float.atan(true)", float.PI/4f, float.atan(true));
AddStrictTestCase("float.atan(false)", 0f, float.atan(false));


AddStrictTestCase("float.atan('1')", float.PI/4f, float.atan('1'));
AddStrictTestCase("float.atan('0')", 0f, float.atan('0'));


