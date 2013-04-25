/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

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

AddStrictTestCase("float.atan2() returns a float", "float", getQualifiedClassName(float.atan2(0f,0f)));
AddStrictTestCase("float.atan2() length is 2", 2, float.atan2.length);

// If either x or y is NaN, the result is NaN.
AddStrictTestCase("float.atan2(1f, float.NaN)", float.NaN, float.atan2(1f, float.NaN));
AddStrictTestCase("float.atan2(float.NaN, 1f)", float.NaN, float.atan2(float.NaN, 1f));
AddStrictTestCase("float.atan2(1f, float.NaN) check()", float.NaN, check(1f, float.NaN));
AddStrictTestCase("float.atan2(float.NaN, 1f) check()", float.NaN, check(float.NaN, 1f));

// If y>0 and x is +0, the result is an implementation-dependent approximation to +PI/2.
AddStrictTestCase("float.atan2(1f, 0f)", float.PI/2f, float.atan2(1f, 0f));
AddStrictTestCase("float.atan2(1f, 0f) check", float.PI/2f, check(1f, 0f));

// If y>0 and x is -0, the result is an implementation-dependent approximation to +PI/2.
AddStrictTestCase("float.atan2(1f, -0f)", float.PI/2f, float.atan2(1f, -0f));
AddStrictTestCase("float.atan2(1f, -0f) check", float.PI/2f, check(1f, -0f));

// If y is +0 and x>0, the result is +0.
Assert.expectEq("float.atan2(0f, 1f)", 0f, float.atan2(0f, 1f));
Assert.expectEq("float.atan2(0f, 1f) check via Infinity", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.atan2(0f, 1f));
Assert.expectEq("float.atan2(0f, 1f) check()", 0f, check(0f, 1f));
Assert.expectEq("float.atan2(0f, 1f) check() check via Infinity", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f, 1f));

// If y is +0 and x is +0, the result is +0.
AddStrictTestCase("float.atan2(0f, 0f)", 0f, float.atan2(0f, 0f));
AddStrictTestCase("float.atan2(0f, 0f) check via Infinity", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.atan2(0f, 0f));
AddStrictTestCase("float.atan2(0f, 0f) check()", 0f, check(0f, 0f));
AddStrictTestCase("float.atan2(0f, 0f) check() check via Infinity", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f, 0f));

// If y is +0 and x is -0, the result is an implementation-dependent approximation to +PI.
AddStrictTestCase("float.atan2(0f, -0f)", float.PI, float.atan2(0f, -0f));
AddStrictTestCase("float.atan2(0f, -0f) check()", float.PI, check(0f, -0f));

// If y is +0 and x<0, the result is an implementation-dependent approximation to +PI.
AddStrictTestCase("float.atan2(0f, -0.1f)", float.PI, float.atan2(0f, -0.1f));
AddStrictTestCase("float.atan2(0f, -0.1f) check()", float.PI, check(0f, -0.1f));

// If y is -0 and x>0, the result is -0.
AddStrictTestCase("float.atan2(-0f, 0.1f)", -0f, float.atan2(-0f, 0.1f));
AddStrictTestCase("float.atan2(-0f, 0.1f) check via Infinity", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.atan2(-0f, 0.1f));
AddStrictTestCase("float.atan2(-0f, 0.1f) check()", -0f, check(-0f, 0.1f));
AddStrictTestCase("float.atan2(-0f, 0.1f) check() check via Infinity", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f, 0.1f));

// If y is -0 and x is +0, the result is -0.
AddStrictTestCase("float.atan2(-0f, 0f)", -0f, float.atan2(-0f, 0f));
AddStrictTestCase("float.atan2(-0f, 0f) check via Infinity", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.atan2(-0f, 0f));
AddStrictTestCase("float.atan2(-0f, 0f) check()", -0f, check(-0f, 0f));
AddStrictTestCase("float.atan2(-0f, 0f) check() check via Infinity", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f, 0f));

// If y is -0 and x is -0, the result is an implementation-dependent approximation to -PI.
AddStrictTestCase("float.atan2(-0f, -0f)", -float.PI, float.atan2(-0f, -0f));
AddStrictTestCase("float.atan2(-0f, -0f) cehck()", -float.PI, check(-0f, -0f));

// If y is -0 and x<0, the result is an implementation-dependent approximation to -PI.
AddStrictTestCase("float.atan2(-0f, -0.1f)", -float.PI, float.atan2(-0f, -0.1f));
AddStrictTestCase("float.atan2(-0f, -0.1f) check()", -float.PI, check(-0f, -0.1f));

// If y<0 and x is +0, the result is an implementation-dependent approximation to -PI/2.
AddStrictTestCase("float.atan2(-0.1f, 0f)", -float.PI/2f, float.atan2(-0.1f, 0f));
AddStrictTestCase("float.atan2(-0.1f, 0f) check()", -float.PI/2f, check(-0.1f, 0f));

// If y<0 and x is -0, the result is an implementation-dependent approximation to -PI/2.
AddStrictTestCase("float.atan2(-0.1f, -0f)", -float.PI/2f, float.atan2(-0.1f, -0f));
AddStrictTestCase("float.atan2(-0.1f, -0f) check()", -float.PI/2f, check(-0.1f, -0f));

// If y>0 and y is finite and x is +Infinity, the result is +0.
AddStrictTestCase("float.atan2(0.1f, float.POSITIVE_INFINITY)", 0f, float.atan2(0.1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(0.1f, float.POSITIVE_INFINITY) check via Infinity",
                  float.POSITIVE_INFINITY,
                  float.POSITIVE_INFINITY/float.atan2(0.1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(0.1f, float.POSITIVE_INFINITY) check()", 0f, check(0.1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(0.1f, float.POSITIVE_INFINITY) check() check via Infinity",
                  float.POSITIVE_INFINITY,
                  float.POSITIVE_INFINITY/check(0.1f, float.POSITIVE_INFINITY));


// If y>0 and y is finite and x is -Infinity, the result if an implementation-dependent approximation to +PI.
AddStrictTestCase("float.atan2(0.1f, float.NEGATIVE_INFINITY)", float.PI, float.atan2(0.1f, float.NEGATIVE_INFINITY));
AddStrictTestCase("float.atan2(0.1f, float.NEGATIVE_INFINITY) check()", float.PI, check(0.1f, float.NEGATIVE_INFINITY));

// If y<0 and y is finite and x is +Infinity, the result is -0.
AddStrictTestCase("float.atan2(-0.1f, float.POSITIVE_INFINITY)", -0f, float.atan2(0.1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(-0.1float, float.POSITIVE_INFINITY) check via Infinity",
                  float.NEGATIVE_INFINITY,
                  float.POSITIVE_INFINITY/float.atan2(-0.1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(-0.1f, float.POSITIVE_INFINITY) check()", -0f, check(0.1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(-0.1f, float.POSITIVE_INFINITY) check() check via Infinity",
                  float.NEGATIVE_INFINITY,
                  float.POSITIVE_INFINITY/check(-0.1f, float.POSITIVE_INFINITY));

// If y<0 and y is finite and x is -Infinity, the result is an implementation-dependent approximation to -PI.
AddStrictTestCase("float.atan2(-0.1f, float.NEGATIVE_INFINITY)", -float.PI, float.atan2(-0.1f, float.NEGATIVE_INFINITY));
AddStrictTestCase("float.atan2(-0.1f, float.NEGATIVE_INFINITY) check()", -float.PI, check(-0.1f, float.NEGATIVE_INFINITY));

// If y is +Infinity and x is finite, the result is an implementation-dependent approximation to +PI/2.
AddStrictTestCase("float.atan2(float.POSITIVE_INFINITY, 0f)", float.PI/2f, float.atan2(float.POSITIVE_INFINITY, 0f));
AddStrictTestCase("float.atan2(float.POSITIVE_INFINITY, 0f) check()", float.PI/2f, check(float.POSITIVE_INFINITY, 0f));

// If y is -Infinity and x is finite, the result is an implementation-dependent approximation to -PI/2.
AddStrictTestCase("float.atan2(float.NEGATIVE_INFINITY, 0f)", -float.PI/2f, float.atan2(float.NEGATIVE_INFINITY, 0f));
AddStrictTestCase("float.atan2(float.NEGATIVE_INFINITY, 0f) check()", -float.PI/2f, check(float.NEGATIVE_INFINITY, 0f));

// If y is +Infinity and x is +Infinity, the result is an implementation-dependent approximation to +PI/4.
AddStrictTestCase("float.atan2(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY)", float.PI/4f, float.atan2(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY) check()", float.PI/4f, check(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY));

// If y is +Infinity and x is -Infinity, the result is an implementation-dependent approximation to +3PI/4.
AddStrictTestCase("float.atan2(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY)", 3f*float.PI/4f, float.atan2(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY));
AddStrictTestCase("float.atan2(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY) check()", 3f*float.PI/4f, check(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY));

// If y is -Infinity and x is +Infinity, the result is an implementation-dependent approximation to -PI/4.
AddStrictTestCase("float.atan2(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY)", -float.PI/4f, float.atan2(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan2(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY) check()", -float.PI/4f, check(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY));

// If y is -Infinity and x is -Infinity, the result is an implementation-dependent approximation to -3PI/4.
AddStrictTestCase("float.atan2(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY)", -3f*float.PI/4f, float.atan2(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY));
AddStrictTestCase("float.atan2(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY) check()", -3f*float.PI/4f, check(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY));


