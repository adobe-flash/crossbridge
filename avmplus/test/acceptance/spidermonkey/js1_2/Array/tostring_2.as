/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'tostring_2.js';

/**
   File Name:          tostring_2.js
   Reference:          http://scopus.mcom.com/bugsplat/show_bug.cgi?id=114564
   Description:        toString in version 120


   Author:             christine@netscape.com
   Date:               15 June 1998
*/

// var SECTION = "Array/tostring_2.js";
// var VERSION = "JS_12";
// var TITLE   = "Array.toString";


var a = [];


Assert.expectEq ( 
	       "a.toString()",
	       "",
	       a.toString() );

Assert.expectEq ( 
	       "String( a )",
	       "",
	       String( a ) );

Assert.expectEq ( 
	       "a +''",
	       "",
	       a+"" );

