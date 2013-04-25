/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'String.js';

/**
   Filename:     String.js
   Description:  'This tests the function String(Object)'

   Author:       Nick Lerissa
   Date:         Fri Feb 13 09:58:28 PST 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'functions: String';


Assert.expectEq(  "String(true)            ",
          'true',  (String(true)));
Assert.expectEq(  "String(false)           ",
          'false',  (String(false)));
Assert.expectEq(  "String(-124)           ",
          '-124',  (String(-124)));
Assert.expectEq(  "String(1.23)          ",
          '1.23',  (String(1.23)));
Assert.expectEq(  "String({p:1})           ",
          '[object Object]',  (String({p:1})));
Assert.expectEq(  "String(null)            ",
          'null',  (String(null)));
Assert.expectEq(  "String([1,2,3])            ",
          '1,2,3',  (String([1,2,3])));


