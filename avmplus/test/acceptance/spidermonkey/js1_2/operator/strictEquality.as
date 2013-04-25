/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'strictEquality.js';

/**
   Filename:     strictEquality.js
   Description:  'This tests the operator ==='

   Author:       Nick Lerissa
   Date:         Fri Feb 13 09:58:28 PST 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'operator "==="';


Assert.expectEq(  "('8' === 8)                              ",
          false,  ('8' === 8));

Assert.expectEq(  "(8 === 8)                                ",
          true,   (8 === 8));

Assert.expectEq(  "(8 === true)                             ",
          false,  (8 === true));

Assert.expectEq(  "(new String('') === new String(''))      ",
          true,  (new String('') === new String('')));

Assert.expectEq(  "(new Boolean(true) === new Boolean(true))",
          true,  (new Boolean(true) === new Boolean(true)));

var anObject = { one:1 , two:2 };

Assert.expectEq(  "(anObject === anObject)                  ",
          true,  (anObject === anObject));

Assert.expectEq(  "(anObject === { one:1 , two:2 })         ",
          false,  (anObject === { one:1 , two:2 }));

Assert.expectEq(  "({ one:1 , two:2 } === anObject)         ",
          false,  ({ one:1 , two:2 } === anObject));

Assert.expectEq(  "(null === null)                          ",
          true,  (null === null));

Assert.expectEq(  "(null === 0)                             ",
          false,  (null === 0));

Assert.expectEq(  "(true === !false)                        ",
          true,  (true === !false));


