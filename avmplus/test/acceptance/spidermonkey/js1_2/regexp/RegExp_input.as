/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'RegExp_input.js';

/**
   Filename:     RegExp_input.js
   Description:  'Tests RegExps input property'

   Author:       Nick Lerissa
   Date:         March 13, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: input';



// RegExp.input = "abcd12357efg"; /\d+/.exec('2345')
RegExp.input = "abcd12357efg";
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; /\\d+/.exec('2345')",
           String(["2345"]), String(/\d+/.exec('2345')));

// RegExp.input = "abcd12357efg"; /\d+/.exec()
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; /\\d+/.exec()",
           String(["12357"]), String(/\d+/.exec("abcd12357efg")));

// RegExp.input = "abcd12357efg"; /[h-z]+/.exec()
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; /[h-z]+/.exec()",
           null, /[h-z]+/.exec("abcd12357efg"));

// RegExp.input = "abcd12357efg"; /\d+/.test('2345')
RegExp.input = "abcd12357efg";
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; /\\d+/.test('2345')",
           true, /\d+/.test('2345'));

// RegExp.input = "abcd12357efg"; /\d+/.test()
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; /\\d+/.test()",
           true, /\d+/.test("abcd12357efg"));

// RegExp.input = "abcd12357efg"; (new RegExp('d+')).test()
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; (new RegExp('d+')).test()",
           true, (new RegExp('d+')).test("abcd12357efg"));

// RegExp.input = "abcd12357efg"; /[h-z]+/.test()
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; /[h-z]+/.test()",
           false, /[h-z]+/.test("abcd12357efg"));

// RegExp.input = "abcd12357efg"; (new RegExp('[h-z]+')).test()
Assert.expectEq (  "RegExp.input = 'abcd12357efg'; (new RegExp('[h-z]+')).test()",
           false, (new RegExp('[h-z]+')).test("abcd12357efg"));

