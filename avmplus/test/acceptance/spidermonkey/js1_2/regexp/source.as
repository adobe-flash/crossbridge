/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'source.js';

/**
   Filename:     source.js
   Description:  'Tests RegExp attribute source'

   Author:       Nick Lerissa
   Date:         March 13, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'RegExp: source';



// /xyz/g.source
Assert.expectEq (  "/xyz/g.source",
           "xyz", /xyz/g.source);

// /xyz/.source
Assert.expectEq (  "/xyz/.source",
           "xyz", /xyz/.source);

// /abc\\def/.source
Assert.expectEq (  "/abc\\\\def/.source",
           "abc\\\\def", /abc\\def/.source);

// /abc[\b]def/.source
Assert.expectEq (  "/abc[\\b]def/.source",
           "abc[\\b]def", /abc[\b]def/.source);

// (new RegExp('xyz')).source
Assert.expectEq (  "(new RegExp('xyz')).source",
           "xyz", (new RegExp('xyz')).source);

// (new RegExp('xyz','g')).source
Assert.expectEq (  "(new RegExp('xyz','g')).source",
           "xyz", (new RegExp('xyz','g')).source);

// (new RegExp('abc\\\\def')).source
Assert.expectEq (  "(new RegExp('abc\\\\\\\\def')).source",
           "abc\\\\def", (new RegExp('abc\\\\def')).source);

// (new RegExp('abc[\\b]def')).source
Assert.expectEq (  "(new RegExp('abc[\\\\b]def')).source",
           "abc[\\b]def", (new RegExp('abc[\\b]def')).source);

