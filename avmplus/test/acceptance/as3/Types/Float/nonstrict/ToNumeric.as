/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "9.3";
// var VERSION = "AS3";
// var TITLE   = "The operation ToNumeric";


var f:float = float(12345.678901);
var n:Number = 12345.678901;

AddStrictTestCase("ToNumeric(Number) is Number strict check", n, +n);
Assert.expectEq("ToNumeric(Number) is Number", "Number",getQualifiedClassName(+n));

AddStrictTestCase("ToNumeric(float) is float strict check", f, +f);
Assert.expectEq("ToNumeric(float) is float", "float",getQualifiedClassName(+f));

AddStrictTestCase("ToNumeric(String) is Number strict check", n, +"12345.678901");
Assert.expectEq("ToNumeric(String) is Number", false, (Infinity == +"123.4567e89")); // when interpreted as float, this yields infinity.

Assert.expectEq("ToNumeric(String) does not produce float for 'f' suffixes", true, isNaN(+"1.0f"));


