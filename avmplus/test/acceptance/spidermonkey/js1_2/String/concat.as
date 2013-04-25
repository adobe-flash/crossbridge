/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'concat.js';

/**
   Filename:     concat.js
   Description:  'This tests the new String object method: concat'

   Author:       NickLerissa
   Date:         Fri Feb 13 09:58:28 PST 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'String:concat';


var aString = new String("test string");
var bString = new String(" another ");

Assert.expectEq(  "aString.concat(' more')", "test string more",     aString.concat(' more').toString());
Assert.expectEq(  "aString.concat(bString)", "test string another ", aString.concat(bString).toString());
Assert.expectEq(  "aString                ", "test string",          aString.toString());
Assert.expectEq(  "bString                ", " another ",            bString.toString());
Assert.expectEq(  "aString.concat(345)    ", "test string345",       aString.concat(345).toString());
Assert.expectEq(  "aString.concat(true)   ", "test stringtrue",      aString.concat(true).toString());
Assert.expectEq(  "aString.concat(null)   ", "test stringnull",      aString.concat(null).toString());
Assert.expectEq(  "aString.concat([])     ", "test string",          aString.concat([]).toString());
Assert.expectEq(  "aString.concat([1,2,3])", "test string1,2,3", aString.concat([1,2,3]).toString());

Assert.expectEq(  "'abcde'.concat(' more')", "abcde more",     'abcde'.concat(' more').toString());
Assert.expectEq(  "'abcde'.concat(bString)", "abcde another ", 'abcde'.concat(bString).toString());
Assert.expectEq(  "'abcde'                ", "abcde",          'abcde');
Assert.expectEq(  "'abcde'.concat(345)    ", "abcde345",       'abcde'.concat(345).toString());
Assert.expectEq(  "'abcde'.concat(true)   ", "abcdetrue",      'abcde'.concat(true).toString());
Assert.expectEq(  "'abcde'.concat(null)   ", "abcdenull",      'abcde'.concat(null).toString());
Assert.expectEq(  "'abcde'.concat([])     ", "abcde",          'abcde'.concat([]).toString());
Assert.expectEq(  "'abcde'.concat([1,2,3])", "abcde1,2,3",     'abcde'.concat([1,2,3]).toString());


Assert.expectEq(  "'abcde'.concat()       ", "abcde",          'abcde'.concat().toString());


