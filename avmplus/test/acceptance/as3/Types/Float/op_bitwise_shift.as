/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.6";
// var VERSION = "AS3";
// var TITLE   = "The bitwise shift operators augmented by float values";


var f1:float = -1.2345678e9; // TODO: use float literals
var f2:float = 3.1234f;
var twof:float = 2f;

Assert.expectEq("left shift on float returns a Number", "Number", getQualifiedClassName(f1<<2));
Assert.expectEq("left shift on float", -643303936, f1<<2);
Assert.expectEq("left shift on FloatLiteral", 4, 1f<<2);
Assert.expectEq("float left shift on float", -643303936, f1<<twof);
Assert.expectEq("FloatLiteral left shift on FloatLiteral", 4, 1f<<2f);
Assert.expectEq("left shift on 3.1234f", 12, f2<<2);
Assert.expectEq("left shift on float.MIN_VALUE", 0, float.MIN_VALUE<<2);
Assert.expectEq("left shift on float.MAX_VALUE", 0, float.MAX_VALUE<<2);



Assert.expectEq("right shift on float returns a Number", "Number", getQualifiedClassName(f1>>2));
Assert.expectEq("right shift on float", -308641952, f1>>2);
Assert.expectEq("right shift on FloatLiteral", 25, 100f>>2);
Assert.expectEq("float right shift on float", -308641952, f1>>twof);
Assert.expectEq("FloatLiteral right shift on FloatLiteral", 25, 100f>>2f);
Assert.expectEq("right shift on 3.1234f", 0, f2>>2);
Assert.expectEq("right shift on float.MIN_VALUE", 0, float.MIN_VALUE>>2);
Assert.expectEq("right shift on float.MAX_VALUE", 0, float.MAX_VALUE>>2);



Assert.expectEq("right shift(unsigned) on float returns a Number", "Number", getQualifiedClassName(f1>>>2));
Assert.expectEq("right shift(unsigned) on float", 765099872, f1>>>2);
Assert.expectEq("right shift(unsigned) on FloatLiteral", 1073741799, -100f>>>2);
Assert.expectEq("float right shift(unsigned) on float", 765099872, f1>>>twof);
Assert.expectEq("FloatLiteral right shift(unsigned) on FloatLiteral", 1073741799, -100f>>>2f);
Assert.expectEq("right shift(unsigned) on 3.1234f", 0, f2>>>2);
Assert.expectEq("right shift(unsigned) on float.MIN_VALUE", 0, float.MIN_VALUE>>>2);
Assert.expectEq("right shift(unsigned) on float.MAX_VALUE", 0, float.MAX_VALUE>>>2);


