/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Writing to read-only globals causes exception to be thrown.

undefined = 10;  print("UNDEFINED " + undefined);
NaN = 10; print("NaN " + NaN);
Infinity = 10; print("Infinity " + Infinity);

x = Object;   Object = 10; print("OBJECT " + (Object === x));
x = Function; Function = 10; print("FUNCTION " + (Function === x));
x = Array;    Array = 10; print("ARRAY " + (Array === x));
x = String;   String = 10; print("STRING " + (String === x));
x = Number;   Number = 10; print("NUMBER " + (Number === x));
x = Boolean;  Boolean = 10; print("BOOLEAN " + (Boolean === x));
x = Date;     Date = 10; print("DATE " + (Date === x));
x = RegExp;   RegExp = 10; print("REGEXP " + (RegExp === x));
x = Error;    Error = 10; print("ERROR " + (Error === x));
x = EvalError; EvalError = 10; print("EVALERROR " + (EvalError === x));
x = RangeError; RangeError = 10; print("RANGEERROR " + (RangeError === x));
x = ReferenceError; ReferenceError = 10; print("REFERENCEERROR " + (ReferenceError === x));
x = SyntaxError; SyntaxError = 10; print("SYNTAXERROR " + (SyntaxError === x));
x = TypeError; TypeError = 10; print("TYPEERROR " + (TypeError === x));
x = URIError; URIError = 10; print("URIERROR " + (URIError === x));
*/

print("DONE");
