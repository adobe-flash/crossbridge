/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.3";
// var VERSION = "AS3";
// var TITLE   = "The float class object called as a function";


/*
When the float class object is called as a function, it performs a type conversion and returns a float
value.
*/

var result = float();
Assert.expectEq("Float with no args", "float", getQualifiedClassName(result));


