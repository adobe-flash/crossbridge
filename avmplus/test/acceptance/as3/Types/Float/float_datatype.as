/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

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



/* TEST: The class type known as float models a 32-bit IEEE single-precision floating-point datum*/
var byteArr:ByteArray = new ByteArray();
var pi_dbl = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
var expected_pi_float:float = 3.1415927f;
var expected_pi_int:int =  1078530011;
var pi_float:float;
var pi_int;

try
{
    byteArr.writeFloat(pi_dbl);
    /// Agree, not very relevant for the Float class per se; but let's leave it here, shall we?
    Assert.expectEq("float is 32bits", 4, byteArr.length);
    byteArr.position = 0;
    pi_float = byteArr.readFloat();
    byteArr.position = 0;
    pi_int =  byteArr.readInt();
}
catch(e)
{
    pi_int = new String(e);
}

Assert.expectEq("float is 32-bit IEEE floating-point", expected_pi_float, pi_float);
Assert.expectEq("float is 32-bit IEEE floating-point (bit representation)", expected_pi_int, pi_int);
/* TEST: It is a direct subtype of Object. */
var pi_obj:Object = pi_float as Object;
Assert.expectEq("float is object", pi_float, pi_obj);

Assert.expectEq("float is direct subtype of Object", "Object", getQualifiedSuperclassName(float));


/* TOTEST: A float instance is immutable (?). Don't know how that could be tested...*/
/* Synopsis: not tested here; there are dedicated sections for float properties */

/*Note(2): In the same way as instances of the class Number appear to the program both as primitive
values and objects, with value equality implying object equality and vice versa, float instances appearboth as primitivevalues and objects.
*/

var new_pi_float = new float(3.1415927);
var expected_obj:Object = expected_pi_float as Object;
var new_obj:Object = new_pi_float as Object;
Assert.expectEq("value equality implies object equality", true, expected_obj === new_obj);

/* // THE FOLOWING TEST IS DISABLED BECAUSE ByteArray.writeObject is not implemented in AVM Shell
try
{
    byteArr.position = 0;
    byteArr.writeObject(expected_pi_float);
    new_obj = byteArr.readObject() as Object;
}
catch(e)
{
    new_obj = new String(e);
}
Assert.expectEq("value equality implies object equality (part 2)", expected_obj, new_obj);
Assert.expectEq("value equality implies object equality (part 2)", true, expected_obj===new_obj);
*/

/* Note(3): As for any other class type, there is a class object for the float type. This class object is
immutably bound to the name "float" in same domain of the environment where "Number" is already
bound.
*/
Assert.expectEq("Class object for type float", false, (float as Object) == null);

var flt_class_name;
try
{
    var flt_cls:Class = float;
    flt_class_name = getQualifiedClassName(flt_cls);
}
catch(e)
{
    flt_class_name = "Exception: "+ new String(e);
}
Assert.expectEq("Class name for type float", "float", flt_class_name);


