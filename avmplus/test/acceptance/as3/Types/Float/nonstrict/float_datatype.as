/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

/*
 4.1 The type float
The class type known as float models a 32-bit IEEE single-precision floating-point datum. It is a direct subtype of Object. A float instance is immutable. Synopsis:
final class float extends Object
{
public static const length:int = ...
public static const MAX_VALUE:float = ...
public static const MIN_VALUE:float = ...
public static const NaN:float = ...
public static const NEGATIVE_INFINITY:float = ...
public static const POSITIVE_INFINITY:float = ...
private static const float_prototype = {
constructor : function (x) ...,
toString : function (radix=10):String ...,
toLocaleString: function (radix=10):String ...,
valueOf : function ():float ...,
toExponential : function (fractionDigits=0):String ...,
toFixed : function (fractionDigits=0):String ...,
toPrecision : function (precision=0):String ...
}
public const prototype:Object = float_prototype
public function float(x) ...
AS3 function toString(radix=10):String ...
AS3 function valueOf():float ...
AS3 function toExponential(fractionDigits=0):String ...
AS3 function toFixed(fractionDigits=0):String ...
AS3 function toPrecision(precision=0):String ....
}
Note(1): The private property "float_prototype" will not be present in an actual implementation; it is a
specification device used here to show that only one unique prototype object is created for the class.
Note(2): In ES3 terms, "const" class and instance properties are DontDelete, DontEnum, and ReadOnly,
while non-"const" class and instance properties are DontEnum and DontDelete. Prototype properties
are DontEnum, though the syntax in the synopsis does not express that.
Note(3): In the same way as instances of the class Number appear to the program both as primitive
values and objects, with value equality implying object equality and vice versa, float instances appearboth as primitive values and objects.
Note(4): As for any other class type, there is a class object for the float type. This class object is
immutably bound to the name "float" in same domain of the environment where "Number" is already
bound.
Note(5): The rest of this specification will use the term "float value" synonymously with the term
"instance of the float class".
*/


// var SECTION = "4.1";
// var VERSION = "AS3";
// var TITLE   = "The type float";


/* check that float instance is immutable - at the very least, we can't write new properties to it */
var expectedError = 1056;
if (as3Enabled)
    expectedError = 1037;

Assert.expectError("float instance is immutable", Utils.REFERENCEERROR+expectedError, function(){   (new float(1)).valueOf = function(){return 0;};  } );


