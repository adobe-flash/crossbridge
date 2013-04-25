/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'tostring_1.js';

/**
   File Name:          tostring_1.js
   ECMA Section:       Array.toString()
   Description:

   This checks the ToString value of Array objects under JavaScript 1.2.

   Author:             christine@netscape.com
   Date:               12 november 1997
*/

// var SECTION = "JS1_2";
// var VERSION = "JS1_2";
// var TITLE   = "Array.toString()";


var a = new Array();

Assert.expectEq ( 
           "var a = new Array(); a.toString()",
            "",
           a.toString() );

a[0] = void 0;

Assert.expectEq ( 
           "a[0] = void 0; a.toString()",
           "",
           a.toString() );


Assert.expectEq( 
          "a.length",
          1,
          a.length );

a[1] = void 0;

Assert.expectEq( 
          "a[1] = void 0; a.toString()",
          ",",
          a.toString() );

a[1] = "hi";

Assert.expectEq( 
          "a[1] = \"hi\"; a.toString()",
          ",hi",
          a.toString() );

a[2] = void 0;

Assert.expectEq( 
          "a[2] = void 0; a.toString()",
          ",hi,",
          a.toString() );

var b = new Array(1000);
var bstring = "";
for ( blen=0; blen<999; blen++) {
  bstring += ",";
}


Assert.expectEq ( 
           "var b = new Array(1000); b.toString()",
           bstring,
           b.toString() );


Assert.expectEq( 
          "b.length",
          1000,
          b.length );

