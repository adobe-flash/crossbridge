/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'regress-7224.js';

/**
 *  File Name:          regress-7224.js
 *  Reference:          js1_2
 *  Description:        Remove support for the arg
 *  Author:             ** replace with your e-mail address **
 */

// var SECTION = "regress";       // provide a document reference (ie, ECMA section)
// var VERSION = "JS1_4"; // Version of JavaScript or ECMA
// var TITLE   = "Regression test for bugzilla #7224";       // Provide ECMA section title or a description
var BUGNUMBER = "http://bugzilla.mozilla.org/show_bug.cgi?id=7224";     // Provide URL to bugsplat or bugzilla report


var f = function() {return arguments.caller};
var o = {};

o.foo = f;
o.foo("a", "b", "c");


Assert.expectEq(
  "var f = new Function( 'return arguments.caller' ); f()",
  undefined,
  f() );

Assert.expectEq(
  "var o = {}; o.foo = f; o.foo('a')",
  undefined,
  o.foo('a') );

// displays results.
