/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'regexparg-2-n.js';

/**
   File Name:          regexparg-1.js
   Description:

   Regression test for
   http://scopus/bugsplat/show_bug.cgi?id=122787
   Passing a regular expression as the first constructor argument fails

   Author:             christine@netscape.com
   Date:               15 June 1998
*/

// var SECTION = "JS_1.2";
// var VERSION = "JS_1.2";
// var TITLE   = "The variable statement";


function f(x) {return x;}

x = f(/abc/);

Assert.expectEq( 
          "function f(x) {return x;}; x = f(/abc/); x",
          /abc/.toString(),
          x.toString() );


