/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'match.js';

/**
   Filename:     match.js
   Description:  'This tests the new String object method: match'

   Author:       NickLerissa
   Date:         Fri Feb 13 09:58:28 PST 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'String:match';


var aString = new String("this is a test string");

Assert.expectEq(  "aString.match(/is.*test/)  ", String(["is is a test"]), String(aString.match(/is.*test/)));
Assert.expectEq(  "aString.match(/s.*s/)  ", String(["s is a test s"]), String(aString.match(/s.*s/)));


