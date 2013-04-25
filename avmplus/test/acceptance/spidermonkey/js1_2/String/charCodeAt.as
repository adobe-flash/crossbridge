/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'charCodeAt.js';

/**
   Filename:     charCodeAt.js
   Description:  'This tests new String object method: charCodeAt'

   Author:       Nick Lerissa
   Date:         Fri Feb 13 09:58:28 PST 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'String:charCodeAt';


var aString = new String("tEs5");

Assert.expectEq(  "aString.charCodeAt(-2)", NaN, aString.charCodeAt(-2));
Assert.expectEq(  "aString.charCodeAt(-1)", NaN, aString.charCodeAt(-1));
Assert.expectEq(  "aString.charCodeAt( 0)", 116, aString.charCodeAt( 0));
Assert.expectEq(  "aString.charCodeAt( 1)",  69, aString.charCodeAt( 1));
Assert.expectEq(  "aString.charCodeAt( 2)", 115, aString.charCodeAt( 2));
Assert.expectEq(  "aString.charCodeAt( 3)",  53, aString.charCodeAt( 3));
Assert.expectEq(  "aString.charCodeAt( 4)", NaN, aString.charCodeAt( 4));
Assert.expectEq(  "aString.charCodeAt( 5)", NaN, aString.charCodeAt( 5));
Assert.expectEq(  "aString.charCodeAt( Infinity)", NaN, aString.charCodeAt( Infinity));
Assert.expectEq(  "aString.charCodeAt(-Infinity)", NaN, aString.charCodeAt(-Infinity));
//Assert.expectEq(  "aString.charCodeAt(  )", 116, aString.charCodeAt( ));


