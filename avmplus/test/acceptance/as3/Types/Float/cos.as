/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "4.5.21";
// var VERSION = "AS3";
// var TITLE   = "public function cos(x:float):float";



AddStrictTestCase("float.cos() returns a float", "float", getQualifiedClassName(float.cos(0)));
AddStrictTestCase("float.cos() length is 1", 1, float.cos.length);

AddStrictTestCase("float.cos(null)", float(1), float.cos(null));
AddStrictTestCase("float.cos(float.NaN)", float.NaN, float.cos(float.NaN));

AddStrictTestCase("float.cos(0)", float(1f), float.cos(0f));
AddStrictTestCase("float.cos(-0)", float(1f), float.cos(-0f));

AddStrictTestCase("float.cos(float.PI)", -float(1), float.cos(float.PI));
AddStrictTestCase("float.cos(-float.PI)", -float(1), float.cos(-float.PI));

var myfloat:float = 3.1415927f;
AddStrictTestCase("float.cos(3.1415927f)", -float(1), float.cos(myfloat));
AddStrictTestCase("float.cos(-3.1415927f)", -float(1), float.cos(-myfloat));

AddStrictTestCase("float.cos(3.1415927f) FloatLiteral", -float(1), float.cos(3.1415927f));
AddStrictTestCase("float.cos(-3.1415927f) FloatLiteral", -float(1), float.cos(-3.1415927f));

AddStrictTestCase("float.cos(float.POSITIVE_INFINITY)", float.NaN, float.cos(float.POSITIVE_INFINITY));
AddStrictTestCase("float.cos(float.NEGATIVE_INFINITY)", float.NaN, float.cos(float.NEGATIVE_INFINITY));

AddStrictTestCase("float.cos(float.MIN_VALUE)", float(1), float.cos(float.MIN_VALUE));


