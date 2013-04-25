/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.2";
// var VERSION = "AS3";
// var TITLE   = "Literal syntax";


/*
The NumericLiteral rule of the lexical grammar (lines 160-161) is extended as follows:
NumericLiteral
...
FloatLiteral
FloatLiteral
DecimalLiteral f
The letter f is the Unicode letter "LATIN-SMALL-LETTER-F", code point 102. (As is normal in the
lexical grammar, no white space or other token separator is allowed between DecimalLiteral and the
letter f.)
For example,
1f
31.4159e-1f
Note: The suffix f appears only in the surface lexical syntax. It is not understood by ToNumeric or
ToFloat. It is not produced by any function that converts a float to a string, notably ToString. Finally,
it is not understood by the parseFloat operator (where it would break backward compatibility by
returning a non-Number for a number value that happened to be followed by the character f, where
that f would previously be uninterpreted). See the rationale for a longer discussion of these issues.
Implementation note: The ABC format has been revised to provide a section for float constant values
as well as a pushfloat instructions.
*/


var one_f = 1f;
var one_float:float = float(1);
Assert.expectEq("literal 1f", one_float, one_f);

var pi_f = 31.4159e-1f;
var pi_float:float = float(31.4159e-1);
Assert.expectEq("Literal 31.4159e-1f", pi_float, pi_f);

/* The "Note" is tested in section 9.8 (operation ToString) and 9.2 (operation ToFloat),
   as well as section 11 (parseFloat). We don't add specific testcases here. */


