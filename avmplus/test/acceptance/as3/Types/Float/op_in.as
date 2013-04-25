/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.7.3";
// var VERSION = "AS3";
// var TITLE   = "The in operator augmented by float values";


var x:String = "";
for (var i in new float(2))
{
    x+= ","+i.toString();
}
x = "["+x+"]";
Assert.expectEq("Enumerable members in float instance", "[]", x);
Assert.expectEq("length in float(1)", false, "length" in float(1));
Assert.expectEq("length in FloatLiteral", false, "length" in 1f);


x = "";
for (var i in float)
    x += ","+ i.toString();
x = "["+x+"]";
Assert.expectEq("Enumerable members in float", "[]", x);
Assert.expectEq("length in float", true, "length" in float);



