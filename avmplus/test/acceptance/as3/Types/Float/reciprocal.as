/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "4.5.29";
// var VERSION = "AS3";
// var TITLE   = "public function reciprocal(x:float):float";


function check(param:float):float { return float.reciprocal(param); }

AddStrictTestCase("float.reciprocal() returns a float", "float", getQualifiedClassName(float.reciprocal(12.345f)));
AddStrictTestCase("float.reciprocal() length is 1", 1, float.reciprocal.length);

// If x is NaN, the result is NaN
AddStrictTestCase("float.reciprocal(float.NaN)", float.NaN, float.reciprocal(float.NaN));
AddStrictTestCase("float.reciprocal(float.NaN) check()", float.NaN, check(float.NaN));

// If x is +Infinity, the result is +0
AddStrictTestCase("float.reciprocal(float.POSITIVE_INFINITY)", float(0), float.reciprocal(float.POSITIVE_INFINITY));
AddStrictTestCase("float.reciprocal(float.POSITIVE_INFINITY) sign check", float.POSITIVE_INFINITY,  1f/float.reciprocal(float.POSITIVE_INFINITY));
AddStrictTestCase("float.reciprocal(float.POSITIVE_INFINITY) check()", float(0), float.reciprocal(float.POSITIVE_INFINITY));
AddStrictTestCase("float.reciprocal(float.POSITIVE_INFINITY) check() sign check", float.POSITIVE_INFINITY, 1f/check(float.POSITIVE_INFINITY));

// If x is -Infinity, the results is -0
AddStrictTestCase("float.reciprocal(float.NEGATIVE_INFINITY)", float(-0), float.reciprocal(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.reciprocal(float.NEGATIVE_INFINITY) sign check", float.NEGATIVE_INFINITY,  1f/float.reciprocal(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.reciprocal(float.NEGATIVE_INFINITY) check()", float(-0), check(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.reciprocal(float.NEGATIVE_INFINITY) check() sign check", float.NEGATIVE_INFINITY,  1f/check(float.NEGATIVE_INFINITY));

// If x is +0, the result is +Infinity
AddStrictTestCase("float.reciprocal(0f)", float.POSITIVE_INFINITY, float.reciprocal(0f));
AddStrictTestCase("float.reciprocal(0f) check()", float.POSITIVE_INFINITY, check(0f));

// If x us -0, the result is -Infinity
AddStrictTestCase("float.reciprocal(-0f)", float.NEGATIVE_INFINITY, float.reciprocal(-0f));
AddStrictTestCase("float.reciprocal(-0f) check", float.NEGATIVE_INFINITY, check(-0f));


AddStrictTestCase("float.reciprocal(8)", float(0.125), float.reciprocal(8));
AddStrictTestCase("float.reciprocal(null)", float.POSITIVE_INFINITY, float.reciprocal(null));


