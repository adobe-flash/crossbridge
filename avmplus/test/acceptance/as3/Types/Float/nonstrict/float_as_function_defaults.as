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


var result = float();
Assert.expectEq("float as function default value +0.0f", 0 , FloatRawBits(result));
Assert.expectEq("float as function default value is NOT -0.0f", true , FloatRawBits(result) != FloatRawBits(-0.0f)); // FloatRawBits(-0.0f) is not 0.

var resultf:float = float();
Assert.expectEq("typed float as function default value +0.0f", 0 , FloatRawBits(resultf));
Assert.expectEq("typed float as function default value is NOT -0.0f", true , FloatRawBits(resultf) != FloatRawBits(-0.0f)); // FloatRawBits(-0.0f) is not 0.


