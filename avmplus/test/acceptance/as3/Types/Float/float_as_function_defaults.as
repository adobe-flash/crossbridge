/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.3.1";
// var VERSION = "AS3";
// var TITLE   = "float ( value=0.0f )";


/*
When the float class object is called as a function, it performs a type conversion and returns a float
value.
4.3.1 float ( value=0.0f )
Returns a float value computed by ToFloat(value).
Note: float(n) is equivalent to new float(n), and both are equivalent to n if n is a float value.
Implementation note:The ABC format has been revised to provide a new convert_f instruction that
implements ToFloat.
*/

var pi_float = float(3.14);
var pi_float_expected1 = 3.14f;
var pi_float_expected2:float = 3.14;
Assert.expectEq("float as function, with 'double' arg", pi_float_expected1, pi_float);
Assert.expectEq("float as function, with 'double' arg typed check", pi_float_expected2, pi_float);

var pi_floatf:float = float(3.14);
Assert.expectEq("typed float as function, with 'double' arg", pi_float_expected1, pi_floatf);
Assert.expectEq("typed float as function, with 'double' arg typed check", pi_float_expected2, pi_floatf);

var three_float = float(3);
var three_float_expected1 = 3f;
var three_float_expected2:float = 3;
Assert.expectEq("float as function, with 'int' arg", three_float_expected1, three_float);
Assert.expectEq("float as function, with 'int' arg typed check", three_float_expected2, three_float);

var three_floatf:float = float(3);
Assert.expectEq("typed float as function, with 'int' arg", three_float_expected1, three_floatf);
Assert.expectEq("typed float as function, with 'int' arg typed check", three_float_expected2, three_floatf);

// Note: not testing new float() - it's tested in section 4.4
/* Also - more complete testing of ToFloat() in section 9.2 */


