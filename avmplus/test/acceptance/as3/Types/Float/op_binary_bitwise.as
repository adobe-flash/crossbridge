/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.9";
// var VERSION = "AS3";
// var TITLE   = "The binary bitwise operators augmented by float values";


var f1:float = -1.2345678e9; // TODO: use float literals
var f2:float = 3.1234f;
var twof:float = 2f;
var val1:float;
var val2:float;

Assert.expectEq("Bitwise AND on floats returns a int", "int", getQualifiedClassName(f1&2));
Assert.expectEq("Bitwise AND on FloatLiteral returns a int", "int", getQualifiedClassName(-1.2345678e9f&3.1223f));
AddStrictTestCase("Bitwise AND on float.MIN_VALUE and float(0xFFFFFF00)", 0, float.MIN_VALUE&float(0xFFFFFF00));
AddStrictTestCase("Bitwise AND on 4294967040 and float(0xFFFFFF00)", int(0xFFFFFF00), 4294967040&float(0xFFFFFF00));
AddStrictTestCase("Bitwise AND on float(0xFFFFFF00) and 4294967040", int(0xFFFFFF00), float(0xFFFFFF00)&4294967040);

val1 = 5f;
val2 = 3f;
AddStrictTestCase("Bitwise AND on 5f and 3f", 1, val1&val2);
AddStrictTestCase("Bitwise AND on FloatLiteral 5f and 3f", 1, 5f&3f);

val1 = 3.125f;
val2 = 2.1f;
AddStrictTestCase("Bitwise AND on 3.125f and 2.1f", 2, val1&val2);
AddStrictTestCase("Bitwise AND on FloatLiteral 3.125f and 2.1f", 2, 3.125f&2.1f);

val1 = 0.62e+1f;
val2 = 131.25e-1f;
AddStrictTestCase("Bitwise AND on 0.62e-1f and 1.3125e+1f", 4, val1&val2);
AddStrictTestCase("Bitwise AND on FloatLiteral 0.62e-1f and 1.3125e+1f", 4, 0.62e+1f&131.25e-1f);



Assert.expectEq("Bitwise XOR on floats returns a int", "int", getQualifiedClassName(twof^2));
Assert.expectEq("Bitwise XOR on FloatLiteral returns a int", "int", getQualifiedClassName(2f^2f));
AddStrictTestCase("Bitwise XOR on float.MIN_VALUE and float(0xFFFFFF00)", int(0xFFFFFF00), float.MIN_VALUE^float(0xFFFFFF00));
AddStrictTestCase("Bitwise XOR on 4294967040 and float(0xFFFFFF00)", 0, 4294967040^float(0xFFFFFF00));
AddStrictTestCase("Bitwise XOR on float(0xFFFFFF00) and 4294967040", 0, float(0xFFFFFF00)^4294967040);

val1 = 5f;
val2 = 3f;
AddStrictTestCase("Bitwise XOR on 5f and 3f", 6, val1^val2);
AddStrictTestCase("Bitwise XOR on FloatLiteral 5f and 3f", 6, 5f^3f);

val1 = 3.125f;
val2 = 2.1f;
AddStrictTestCase("Bitwise XOR on 3.125f and 2.1f", 1, val1^val2);
AddStrictTestCase("Bitwise XOR on FloatLiteral 3.125f and 2.1f", 1, 3.125f^2.1f);

val1 = 0.62e+1f;
val2 = 131.25e-1f;
AddStrictTestCase("Bitwise XOR on 0.62e-1f and 1.3125e+1f", 11, val1^val2);
AddStrictTestCase("Bitwise XOR on FloatLiteral 0.62e-1f and 1.3125e+1f", 11, 0.62e+1f^131.25e-1f);


Assert.expectEq("Bitwise OR on floats returns a int", "int", getQualifiedClassName(twof|2));
Assert.expectEq("Bitwise OR on FloatLiteral returns a int", "int", getQualifiedClassName(2f|2f));
AddStrictTestCase("Bitwise OR on float.MIN_VALUE and float(0xFFFFFF00)", int(0xFFFFFF00), float.MIN_VALUE|float(0xFFFFFF00));
AddStrictTestCase("Bitwise OR on 4294967040 and float(0xFFFFFF00)", -256, 4294967040|float(0xFFFFFF00));
AddStrictTestCase("Bitwise OR on float(0xFFFFFF00) and 4294967040", -256, float(0xFFFFFF00)|4294967040);

val1 = 5f;
val2 = 3f;
AddStrictTestCase("Bitwise OR on 5f and 3f", 7, val1|val2);
AddStrictTestCase("Bitwise OR on FloatLiteral 5f and 3f", 7, 5f|3f);

val1 = 3.125f;
val2 = 2.1f;
AddStrictTestCase("Bitwise OR on 3.125f and 2.1f", 3, val1|val2);
AddStrictTestCase("Bitwise OR on FloatLiteral 3.125f and 2.1f", 3, 3.125f|2.1f);

val1 = 0.62e+1f;
val2 = 131.25e-1f;
AddStrictTestCase("Bitwise OR on 0.62e-1f and 1.3125e+1f", 15, val1|val2);
AddStrictTestCase("Bitwise OR on FloatLiteral 0.62e-1f and 1.3125e+1f", 15, 0.62e+1f|131.25e-1f);




