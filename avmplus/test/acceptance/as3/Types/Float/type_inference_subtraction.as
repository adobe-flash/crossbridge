/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.2.2a";
// var VERSION = "AS3";
// var TITLE   = "Type inference for constant values subtraction tests";


/*
The evaluated (constant) value val of the tree v can be represented in the value set of float, that is,
val is an integer whose absolute value is in the range [0,2^127] and in whose binary representation all
the bits following the 24 most significant bits are zero.

a) It is one of the two operands to one of the binary operators +, -, *, /, %, <, <=, >, >=, ==, ===, !=,
or !==, and the static type of the other operand is float.
*/

/*
values: NumericLiteral that should be coerced to float
0
1
10
100
1.0e+2
1.6777216e+7
1.0
0.0
negative versions


v1 or v2 is the  above value and the other is a float

var foo:*;
foo = v1 op v2;
typeOf foo -> float if the conversion is allowed, otherwise Number
*/


var onef:float = 1f;

var addfn_val1:*;
addfn_val1 = onef - 0;
Assert.expectEq("float - 0 NumberLiteral", "float", typeof addfn_val1);
var addfn_val1Literal:*;
addfn_val1Literal = 1f - 0;
Assert.expectEq("FloatLiteral - 0 NumberLiteral", "float", typeof addfn_val1Literal);
var addnf_val1:*;
addnf_val1 = 0 - onef;
Assert.expectEq("0 NumberLiteral - float", "float", typeof addnf_val1);
var addnf_valLiteral1:*;
addnf_valLiteral1 = 0 - 1f;
Assert.expectEq("0 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral1);

var addfn_val2:*;
addfn_val2 = onef - 1;
Assert.expectEq("float - 1 NumberLiteral", "float", typeof addfn_val2);
var addfn_val2Literal:*;
addfn_val2Literal = 1f - 1;
Assert.expectEq("FloatLiteral - 1 NumberLiteral", "float", typeof addfn_val2Literal);
var addnf_val2:*;
addnf_val2 = 1 - onef;
Assert.expectEq("1 NumberLiteral - float", "float", typeof addnf_val2);
var addnf_valLiteral2:*;
addnf_valLiteral2 = 1 - 1f;
Assert.expectEq("1 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral2);

var addfn_val3:*;
addfn_val3 = onef - 10;
Assert.expectEq("float - 10 NumberLiteral", "float", typeof addfn_val3);
var addfn_val3Literal:*;
addfn_val3Literal = 1f - 10;
Assert.expectEq("FloatLiteral - 10 NumberLiteral", "float", typeof addfn_val3Literal);
var addnf_val3:*;
addnf_val3 = 10 - onef;
Assert.expectEq("10 NumberLiteral - float", "float", typeof addnf_val3);
var addnf_valLiteral3:*;
addnf_valLiteral3 = 10 - 1f;
Assert.expectEq("10 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral3);

var addfn_val4:*;
addfn_val4 = onef - 100;
Assert.expectEq("float - 100 NumberLiteral", "float", typeof addfn_val4);
var addfn_val4Literal:*;
addfn_val4Literal = 1f - 100;
Assert.expectEq("FloatLiteral - 100 NumberLiteral", "float", typeof addfn_val4Literal);
var addnf_val4:*;
addnf_val4 = 100 - onef;
Assert.expectEq("100 NumberLiteral - float", "float", typeof addnf_val4);
var addnf_valLiteral4:*;
addnf_valLiteral4 = 100 - 1f;
Assert.expectEq("100 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral4);

var addfn_val5:*;
addfn_val5 = onef - 1.0e+2;
Assert.expectEq("float - 1.0e+2 NumberLiteral", "float", typeof addfn_val5);
var addfn_val5Literal:*;
addfn_val5Literal = 1f - 1.0e+2;
Assert.expectEq("FloatLiteral - 1.0e+2 NumberLiteral", "float", typeof addfn_val5Literal);
var addnf_val5:*;
addnf_val5 = 1.0e+2 - onef;
Assert.expectEq("1.0e+2 NumberLiteral - float", "float", typeof addnf_val5);
var addnf_valLiteral5:*;
addnf_valLiteral5 = 1.0e+2 - 1f;
Assert.expectEq("1.0e+2 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral5);

var addfn_val6:*;
addfn_val6 = onef - 1.6777216e+7;
Assert.expectEq("float - 1.6777216e+7", "float", typeof addfn_val6);
var addfn_val6Literal:*;
addfn_val6Literal = 1f - 1.6777216e+7;
Assert.expectEq("FloatLiteral - 1.6777216e+7", "float", typeof addfn_val6Literal);
var addnf_val6:*;
addnf_val6 = 1.6777216e+7 - onef;
Assert.expectEq("1.6777216e+7 - float", "float", typeof addnf_val6);
var addnf_valLiteral6:*;
addnf_valLiteral6 = 1.6777216e+7 - 1f;
Assert.expectEq("2.147483647e+9 - FloatLiteral", "float", typeof addnf_valLiteral6);

var addfn_val7:*;
addfn_val7 = onef - 1.0;
Assert.expectEq("float - 1.0 NumberLiteral", "float", typeof addfn_val7);
var addfn_val7Literal:*;
addfn_val7Literal = 1f - 1.0;
Assert.expectEq("FloatLiteral - 1.0 NumberLiteral", "float", typeof addfn_val7Literal);
var addnf_val7:*;
addnf_val7 = 1.0 - onef;
Assert.expectEq("1.0 NumberLiteral - float", "float", typeof addnf_val7);
var addnf_valLiteral7:*;
addnf_valLiteral7 = 1.0 - 1f;
Assert.expectEq("1.0 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral7);

var addfn_val8:*;
addfn_val8 = onef - 0.0;
Assert.expectEq("float - 0.0 NumberLiteral", "float", typeof addfn_val8);
var addfn_val8Literal:*;
addfn_val8Literal = 1f - 0.0;
Assert.expectEq("FloatLiteral - 0.0 NumberLiteral", "float", typeof addfn_val8Literal);
var addnf_val8:*;
addnf_val8 = 0.0 - onef;
Assert.expectEq("0.0 NumberLiteral - float", "float", typeof addnf_val8);
var addnf_valLiteral8:*;
addnf_valLiteral8 = 0.0 - 1f;
Assert.expectEq("0.0 NumberLiteral - FloatLiteral", "float", typeof addnf_valLiteral8);



var neg_addfn_val1:*;
neg_addfn_val1 = onef - (-0);
Assert.expectEq("float - (-0) NumberLiteral", "float", typeof neg_addfn_val1);
var neg_addfn_val1Literal:*;
neg_addfn_val1Literal = 1f - (-0);
Assert.expectEq("FloatLiteral - (-0) NumberLiteral", "float", typeof neg_addfn_val1Literal);
var neg_addnf_val1:*;
neg_addnf_val1 = -0 - onef;
Assert.expectEq("-0 NumberLiteral - float", "float", typeof neg_addnf_val1);
var neg_addnf_valLiteral1:*;
neg_addnf_valLiteral1 = -0 - 1f;
Assert.expectEq("-0 NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral1);

var neg_addfn_val2:*;
neg_addfn_val2 = onef - (-1);
Assert.expectEq("float - (-1) NumberLiteral", "float", typeof neg_addfn_val2);
var neg_addfn_val2Literal:*;
neg_addfn_val2Literal = 1f - (-1);
Assert.expectEq("FloatLiteral - (-1) NumberLiteral", "float", typeof neg_addfn_val2Literal);
var neg_addnf_val2:*;
neg_addnf_val2 = (-1) - onef;
Assert.expectEq("(-1) NumberLiteral - float", "float", typeof neg_addnf_val2);
var neg_addnf_valLiteral2:*;
neg_addnf_valLiteral2 = (-1) - 1f;
Assert.expectEq("(-1) NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral2);

var neg_addfn_val3:*;
neg_addfn_val3 = onef - (-10);
Assert.expectEq("float - (-10) NumberLiteral", "float", typeof neg_addfn_val3);
var neg_addfn_val3Literal:*;
neg_addfn_val3Literal = 1f - (-10);
Assert.expectEq("FloatLiteral - (-10) NumberLiteral", "float", typeof neg_addfn_val3Literal);
var neg_addnf_val3:*;
neg_addnf_val3 = (-10) - onef;
Assert.expectEq("(-10) NumberLiteral - float", "float", typeof neg_addnf_val3);
var neg_addnf_valLiteral3:*;
neg_addnf_valLiteral3 = (-10) - 1f;
Assert.expectEq("(-10) NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral3);

var neg_addfn_val4:*;
neg_addfn_val4 = onef - (-100);
Assert.expectEq("float - (-100) NumberLiteral", "float", typeof neg_addfn_val4);
var neg_addfn_val4Literal:*;
neg_addfn_val4Literal = 1f - (-100);
Assert.expectEq("FloatLiteral - (-100) NumberLiteral", "float", typeof neg_addfn_val4Literal);
var neg_addnf_val4:*;
neg_addnf_val4 = (-100) - onef;
Assert.expectEq("(-100) NumberLiteral - float", "float", typeof neg_addnf_val4);
var neg_addnf_valLiteral4:*;
neg_addnf_valLiteral4 = (-100) - 1f;
Assert.expectEq("(-100) NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral4);

var neg_addfn_val5:*;
neg_addfn_val5 = onef - (-1.0e+2);
Assert.expectEq("float - (-1.0e+2) NumberLiteral", "float", typeof neg_addfn_val5);
var neg_addfn_val5Literal:*;
neg_addfn_val5Literal = 1f - (-1.0e+2);
Assert.expectEq("FloatLiteral - (-1.0e+2) NumberLiteral", "float", typeof neg_addfn_val5Literal);
var neg_addnf_val5:*;
neg_addnf_val5 = (-1.0e+2) - onef;
Assert.expectEq("(-1.0e+2) NumberLiteral - float", "float", typeof neg_addnf_val5);
var neg_addnf_valLiteral5:*;
neg_addnf_valLiteral5 = (-1.0e+2) - 1f;
Assert.expectEq("(-1.0e+2) NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral5);

var neg_addfn_val6:*;
neg_addfn_val6 = onef - (-1.6777216e+7);
Assert.expectEq("float - (-1.6777216e+7)", "float", typeof neg_addfn_val6);
var neg_addfn_val6Literal:*;
neg_addfn_val6Literal = 1f - (-1.6777216e+7);
Assert.expectEq("FloatLiteral - (-1.6777216e+7)", "float", typeof neg_addfn_val6Literal);
var neg_addnf_val6:*;
neg_addnf_val6 = (-1.6777216e+7) - onef;
Assert.expectEq("(-1.6777216e+7) - float", "float", typeof neg_addnf_val6);
var neg_addnf_valLiteral6:*;
neg_addnf_valLiteral6 = (-1.6777216e+7) - 1f;
Assert.expectEq("(-1.6777216e+7) - FloatLiteral", "float", typeof neg_addnf_valLiteral6);

var neg_addfn_val7:*;
neg_addfn_val7 = onef - -1;
Assert.expectEq("float - -1 NumberLiteral", "float", typeof neg_addfn_val7);
var neg_addfn_val7Literal:*;
neg_addfn_val7Literal = 1f - -1;
Assert.expectEq("FloatLiteral - -1 NumberLiteral", "float", typeof neg_addfn_val7Literal);
var neg_addnf_val7:*;
neg_addnf_val7 = -1 - onef;
Assert.expectEq("-1 NumberLiteral - float", "float", typeof neg_addnf_val7);
var neg_addnf_valLiteral7:*;
neg_addnf_valLiteral7 = -1 - 1f;
Assert.expectEq("-1 NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral7);

var neg_addfn_val8:*;
neg_addfn_val8 = onef - -0;
Assert.expectEq("float - -0 NumberLiteral", "float", typeof neg_addfn_val8);
var neg_addfn_val8Literal:*;
neg_addfn_val8Literal = 1f - -0;
Assert.expectEq("FloatLiteral - -0 NumberLiteral", "float", typeof neg_addfn_val8Literal);
var neg_addnf_val8:*;
neg_addnf_val8 = -0 - onef;
Assert.expectEq("-0 NumberLiteral - float", "float", typeof neg_addnf_val8);
var neg_addnf_valLiteral8:*;
neg_addnf_valLiteral8 = -0 - 1f;
Assert.expectEq("-0 NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral8);

var neg_addfn_val9:*;
neg_addfn_val9 = onef - (-1.0);
Assert.expectEq("float - (-1.0) NumberLiteral", "float", typeof neg_addfn_val9);
var neg_addfn_val9Literal:*;
neg_addfn_val9Literal = 1f - (-1.0);
Assert.expectEq("FloatLiteral - (-1.0) NumberLiteral", "float", typeof neg_addfn_val9Literal);
var neg_addnf_val9:*;
neg_addnf_val9 = (-1.0) - onef;
Assert.expectEq("(-1.0) NumberLiteral - float", "float", typeof neg_addnf_val9);
var neg_addnf_valLiteral9:*;
neg_addnf_valLiteral9 = (-1.0) - 1f;
Assert.expectEq("(-1.0) NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral9);

var neg_addfn_val10:*;
neg_addfn_val10 = onef - (-0.0);
Assert.expectEq("float - (-0.0) NumberLiteral", "float", typeof neg_addfn_val10);
var neg_addfn_val10Literal:*;
neg_addfn_val10Literal = 1f - (-0.0);
Assert.expectEq("FloatLiteral - (-0.0) NumberLiteral", "float", typeof neg_addfn_val10Literal);
var neg_addnf_val10:*;
neg_addnf_val10 = (-0.0) - onef;
Assert.expectEq("(-0.0) NumberLiteral - float", "float", typeof neg_addnf_val10);
var neg_addnf_valLiteral10:*;
neg_addnf_valLiteral10 = (-0.0) - 1f;
Assert.expectEq("(-0.0) NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral10);

var neg_addfn_val11:*;
neg_addfn_val11 = onef - -0.0;
Assert.expectEq("float - -0.0 NumberLiteral", "float", typeof neg_addfn_val11);
var neg_addfn_val11Literal:*;
neg_addfn_val11Literal = 1f - -0.0;
Assert.expectEq("FloatLiteral - -0.0 NumberLiteral", "float", typeof neg_addfn_val11Literal);
var neg_addnf_val11:*;
neg_addnf_val11 = -0.0 - onef;
Assert.expectEq("-0.0 NumberLiteral - float", "float", typeof neg_addnf_val11);
var neg_addnf_valLiteral11:*;
neg_addnf_valLiteral11 = -0.0 - 1f;
Assert.expectEq("-0.0 NumberLiteral - FloatLiteral", "float", typeof neg_addnf_valLiteral11);



