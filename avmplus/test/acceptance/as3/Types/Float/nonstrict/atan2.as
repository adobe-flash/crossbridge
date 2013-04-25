/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
Returns an implementation-dependent approximation to the arc tangent of the
quotient y/x of the arguments y and x, where the signs of y and x are used to
determine the quadrant of the result. Note that it is intentional and traditional
for the two-argument arc tangent function that the argument named y be first and
the argument named x be second. The result is expressed in radians and ranges
from -PI to +PI.
*/

// var SECTION = "4.5.19";
// var VERSION = "AS3";
// var TITLE   = "public function atan2(y:float,x:float):float";


function check(param1:float, param2:float):float { return float.atan2(param1, param2); }

Assert.expectError("float.atan2() with no args", Utils.ARGUMENTERROR+1063,  function(){ float.atan2(); });
Assert.expectError("float.atan2(0) with one args", Utils.ARGUMENTERROR+1063,  function(){ float.atan2(0); });

// If either x or y is NaN, the result is NaN.
AddStrictTestCase("float.atan2(0f, string)", float.NaN, float.atan2(0f, "string"));
AddStrictTestCase("float.atan2(string, 0f)", float.NaN, float.atan2("string", 0f));

AddStrictTestCase("float.atan2(0f, undefined)", float.NaN, float.atan2(0f, undefined));
AddStrictTestCase("float.atan2(undefined, 0f)", float.NaN, float.atan2(undefined, 0f));

// If y>0 and x is +0, the result is an implementation-dependent approximation to +PI/2.
AddStrictTestCase("float.atan2('1', '0')", float.PI/2f, float.atan2('1', '0'));

// If y>0 and x is -0, the result is an implementation-dependent approximation to +PI/2.
AddStrictTestCase("float.atan2('1', '-0')", float.PI/2f, float.atan2('1', '-0'));


