/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.1";
// var VERSION = "AS3";
// var TITLE   = "new float ( value=0.0f )";


/*
4.4.1 new float ( value=0.0f )
Returns a float value computed by ToFloat(value).
*/

var result = new float();
Assert.expectEq("float constructor default value +0.0f", 0 , FloatRawBits(result));
Assert.expectEq("float constructor default value is NOT -0.0f", true , FloatRawBits(result) != FloatRawBits(-0.0f)); // FloatRawBits(-0.0f) is not 0.

var resultf:float = new float();
Assert.expectEq("typed float as function default value +0.0f", 0 , FloatRawBits(resultf));
Assert.expectEq("typed float as function default value is NOT -0.0f", true , FloatRawBits(resultf) != FloatRawBits(-0.0f)); // FloatRawBits(-0.0f) is not 0.


var pi_float = new float(3.14);
var pi_float_expected1 = 3.14f;
var pi_float_expected2:float = 3.14;
Assert.expectEq("Float constructor, with 'double' arg", pi_float_expected1, pi_float);
Assert.expectEq("Float constructor, with 'double' arg type checked", pi_float_expected2, pi_float);

var pi_floatf:float = new float(3.14);
Assert.expectEq("typed float as function, with 'double' arg", pi_float_expected1, pi_floatf);
Assert.expectEq("typed float as function, with 'double' arg typed check", pi_float_expected2, pi_floatf);

var three_float = new float(3);
var three_float_expected1 = 3f;
var three_float_expected2:float = 3;
Assert.expectEq("float as function, with 'int' arg", three_float_expected1, three_float);
Assert.expectEq("float as function, with 'int' arg typed check", three_float_expected2, three_float);

var three_floatf:float = new float(3);
Assert.expectEq("typed float as function, with 'int' arg", three_float_expected1, three_floatf);
Assert.expectEq("typed float as function, with 'int' arg typed check", three_float_expected2, three_floatf);


/* Also - more complete testing of ToFloat() in section 9.2 */


