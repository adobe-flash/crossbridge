/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'string_split.js';

/**
   Filename:     string_split.js
   Description:  'Tests the split method on Strings using regular expressions'

   Author:       Nick Lerissa
   Date:         March 11, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'String: split';



// 'a b c de f'.split(/\s/)
Assert.expectEq (  "'a b c de f'.split(/\s/)",
           String(["a","b","c","de","f"]), String('a b c de f'.split(/\s/)));

// 'a b c de f'.split(/\s/,3)
Assert.expectEq (  "'a b c de f'.split(/\s/,3)",
           String(["a","b","c"]), String('a b c de f'.split(/\s/,3)));

// 'a b c de f'.split(/X/)
Assert.expectEq (  "'a b c de f'.split(/X/)",
           String(["a b c de f"]), String('a b c de f'.split(/X/)));

// 'dfe23iu 34 =+65--'.split(/\d+/)
Assert.expectEq (  "'dfe23iu 34 =+65--'.split(/\d+/)",
           String(["dfe","iu "," =+","--"]), String('dfe23iu 34 =+65--'.split(/\d+/)));

// 'dfe23iu 34 =+65--'.split(new RegExp('\d+'))
Assert.expectEq (  "'dfe23iu 34 =+65--'.split(new RegExp('\\d+'))",
           String(["dfe","iu "," =+","--"]), String('dfe23iu 34 =+65--'.split(new RegExp('\\d+'))));

// 'abc'.split(/[a-z]/)
Assert.expectEq (  "'@ab*c'.split(/[a-z]/)",
           String(["@","","*",""]), String('@ab*c'.split(/[a-z]/)));

// 'abc'.split(/[a-z]/)
Assert.expectEq (  "'a!b!c'.split(/[a-z]/)",
           String(["","!","!","!"]), String('a!b!c!'.split(/[a-z]/)));

// 'abc'.split(new RegExp('[a-z]'))
Assert.expectEq (  "'!@a\(bc&&'.split(new RegExp('[a-z]'))",
           String(["!@","(","","&&"]), String('!@a\(bc&&'.split(new RegExp('[a-z]'))));

// 'abc'.split(new RegExp('[a-z]'))
Assert.expectEq (  "'\(abc\)'.split(new RegExp('[a-z]'))",
           String(["(","","",")"]), String('\(abc\)'.split(new RegExp('[a-z]'))));

