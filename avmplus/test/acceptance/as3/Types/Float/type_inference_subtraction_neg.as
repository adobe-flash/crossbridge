/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.2.2a";
// var VERSION = "AS3";
// var TITLE   = "Type inference for constant values subtraction negative tests";


/*
The evaluated (constant) value val of the tree v can be represented in the value set of float, that is,
val is an integer whose absolute value is in the range [0,2^127] and in whose binary representation all
the bits following the 24 most significant bits are zero.

a) It is one of the two operands to one of the binary operators +, -, *, /, %, <, <=, >, >=, ==, ===, !=,
or !==, and the static type of the other operand is float.
*/

/*
values: NumericalLiteral that should NOT be coerced to float
0.1
1.6777217e+7
1.1
Infinity
negative versions

v1 or v2 is the  above value and the other is a float

var foo:*;
foo = v1 op v2;
typeOf foo -> float if the conversion is allowed, otherwise Number
*/


var onef:float = 1f;

var addfn_val1:*;
addfn_val1 = onef - 0.1;
Assert.expectEq("float - 0.1 NumberLiteral", "number", typeof addfn_val1);
var addfn_val1Literal:*;
addfn_val1Literal = 1f - 0.1;
Assert.expectEq("FloatLiteral - 0.1 NumberLiteral", "number", typeof addfn_val1Literal);
var addnf_val1:*;
addnf_val1 = 0.1 - onef;
Assert.expectEq("0.1 NumberLiteral - float", "number", typeof addnf_val1);
var addnf_valLiteral1:*;
addnf_valLiteral1 = 0.1 - 1f;
Assert.expectEq("0.1 NumberLiteral - FloatLiteral", "number", typeof addnf_valLiteral1);

var addfn_val2:*;
addfn_val2 = onef - 1.6777217e+7;
Assert.expectEq("float - 1.6777217e+7 NumberLiteral", "number", typeof addfn_val2);
var addfn_val2Literal:*;
addfn_val2Literal = 1f - 1.6777217e+7;
Assert.expectEq("FloatLiteral - 1.6777217e+7 NumberLiteral", "number", typeof addfn_val2Literal);
var addnf_val2:*;
addnf_val2 = 1.6777217e+7 - onef;
Assert.expectEq("1.6777217e+7 NumberLiteral - float", "number", typeof addnf_val2);
var addnf_valLiteral2:*;
addnf_valLiteral2 = 1.6777217e+7 - 1f;
Assert.expectEq("1.6777217e+7 NumberLiteral - FloatLiteral", "number", typeof addnf_valLiteral2);

var addfn_val4:*;
addfn_val4 = onef - 1.1;
Assert.expectEq("float - 1.1 NumberLiteral", "number", typeof addfn_val4);
var addfn_val4Literal:*;
addfn_val4Literal = 1f - 1.1;
Assert.expectEq("FloatLiteral - 1.1 NumberLiteral", "number", typeof addfn_val4Literal);
var addnf_val4:*;
addnf_val4 = 1.1 - onef;
Assert.expectEq("1.1 NumberLiteral - float", "number", typeof addnf_val4);
var addnf_valLiteral4:*;
addnf_valLiteral4 = 1.1 - 1f;
Assert.expectEq("1.1 NumberLiteral - FloatLiteral", "number", typeof addnf_valLiteral4);

var addfn_val6:*;
addfn_val6 = onef - Infinity;
Assert.expectEq("float - Infinity", "number", typeof addfn_val6);
var addfn_val6Literal:*;
addfn_val6Literal = 1f - Infinity;
Assert.expectEq("FloatLiteral - Infinity", "number", typeof addfn_val6Literal);
var addnf_val6:*;
addnf_val6 = Infinity - onef;
Assert.expectEq("Infinity - float", "number", typeof addnf_val6);
var addnf_valLiteral6:*;
addnf_valLiteral6 = Infinity - 1f;
Assert.expectEq("Infinity - FloatLiteral", "number", typeof addnf_valLiteral6);

var neg_addfn_val1:*;
neg_addfn_val1 = onef - (-0.1);
Assert.expectEq("float - (-0.1) NumberLiteral", "number", typeof neg_addfn_val1);
var neg_addfn_val1Literal:*;
neg_addfn_val1Literal = 1f - (-0.1);
Assert.expectEq("FloatLiteral - (-0.1) NumberLiteral", "number", typeof neg_addfn_val1Literal);
var neg_addnf_val1:*;
neg_addnf_val1 = -0.1 - onef;
Assert.expectEq("-0.1 NumberLiteral - float", "number", typeof neg_addnf_val1);
var neg_addnf_valLiteral1:*;
neg_addnf_valLiteral1 = -0.1 - 1f;
Assert.expectEq("-0.1 NumberLiteral - FloatLiteral", "number", typeof neg_addnf_valLiteral1);

var neg_addfn_val2:*;
neg_addfn_val2 = onef - (-1.6777217e+7);
Assert.expectEq("float - (-1.6777217e+7) NumberLiteral", "number", typeof neg_addfn_val2);
var neg_addfn_val2Literal:*;
neg_addfn_val2Literal = 1f - (-1.6777217e+7);
Assert.expectEq("FloatLiteral - (-1.6777217e+7) NumberLiteral", "number", typeof neg_addfn_val2Literal);
var neg_addnf_val2:*;
neg_addnf_val2 = (-1.6777217e+7) - onef;
Assert.expectEq("(-1.6777217e+7) NumberLiteral - float", "number", typeof neg_addnf_val2);
var neg_addnf_valLiteral2:*;
neg_addnf_valLiteral2 = (-1.6777217e+7) - 1f;
Assert.expectEq("(-1.6777217e+7) NumberLiteral - FloatLiteral", "number", typeof neg_addnf_valLiteral2);

var neg_addfn_val4:*;
neg_addfn_val4 = onef - (-1.1);
Assert.expectEq("float - (-1.1) NumberLiteral", "number", typeof neg_addfn_val4);
var neg_addfn_val4Literal:*;
neg_addfn_val4Literal = 1f - (-1.1);
Assert.expectEq("FloatLiteral - (-1.1) NumberLiteral", "number", typeof neg_addfn_val4Literal);
var neg_addnf_val4:*;
neg_addnf_val4 = (-1.1) - onef;
Assert.expectEq("(-1.1) NumberLiteral - float", "number", typeof neg_addnf_val4);
var neg_addnf_valLiteral4:*;
neg_addnf_valLiteral4 = (-1.1) - 1f;
Assert.expectEq("(-1.1) NumberLiteral - FloatLiteral", "number", typeof neg_addnf_valLiteral4);

var neg_addfn_val6:*;
neg_addfn_val6 = onef - (-Infinity);
Assert.expectEq("float - (-Infinity)", "number", typeof neg_addfn_val6);
var neg_addfn_val6Literal:*;
neg_addfn_val6Literal = 1f - (-Infinity);
Assert.expectEq("FloatLiteral - (-Infinity)", "number", typeof neg_addfn_val6Literal);
var neg_addnf_val6:*;
neg_addnf_val6 = (-Infinity) - onef;
Assert.expectEq("(-Infinity) - float", "number", typeof neg_addnf_val6);
var neg_addnf_valLiteral6:*;
neg_addnf_valLiteral6 = (-Infinity) - 1f;
Assert.expectEq("(-Infinity) - FloatLiteral", "number", typeof neg_addnf_valLiteral6);

var neg_addfn_val7:*;
neg_addfn_val7 = onef - -1.1;
Assert.expectEq("float - -1.1 NumberLiteral", "number", typeof neg_addfn_val7);
var neg_addfn_val7Literal:*;
neg_addfn_val7Literal = 1f - -1.1;
Assert.expectEq("FloatLiteral - -1.1 NumberLiteral", "number", typeof neg_addfn_val7Literal);
var neg_addnf_val7:*;
neg_addnf_val7 = -1.1 - onef;
Assert.expectEq("-1.1 NumberLiteral - float", "number", typeof neg_addnf_val7);
var neg_addnf_valLiteral7:*;
neg_addnf_valLiteral7 = -1.1 - 1f;
Assert.expectEq("-1.1 NumberLiteral - FloatLiteral", "number", typeof neg_addnf_valLiteral7);


