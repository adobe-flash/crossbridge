/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'dot.js';

/**
   Filename:     dot.js
   Description:  'Tests regular expressions containing .'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'RegExp: .';



// 'abcde'.match(new RegExp('ab.de'))
Assert.expectEq (  "'abcde'.match(new RegExp('ab.de'))",
           String(["abcde"]), String('abcde'.match(new RegExp('ab.de'))));

// 'line 1\nline 2'.match(new RegExp('.+'))
Assert.expectEq (  "'line 1\nline 2'.match(new RegExp('.+'))",
           String(["line 1"]), String('line 1\nline 2'.match(new RegExp('.+'))));

// 'this is a test'.match(new RegExp('.*a.*'))
Assert.expectEq (  "'this is a test'.match(new RegExp('.*a.*'))",
           String(["this is a test"]), String('this is a test'.match(new RegExp('.*a.*'))));

// 'this is a *&^%$# test'.match(new RegExp('.+'))
Assert.expectEq (  "'this is a *&^%$# test'.match(new RegExp('.+'))",
           String(["this is a *&^%$# test"]), String('this is a *&^%$# test'.match(new RegExp('.+'))));

// '....'.match(new RegExp('.+'))
Assert.expectEq (  "'....'.match(new RegExp('.+'))",
           String(["...."]), String('....'.match(new RegExp('.+'))));

// 'abcdefghijklmnopqrstuvwxyz'.match(new RegExp('.+'))
Assert.expectEq (  "'abcdefghijklmnopqrstuvwxyz'.match(new RegExp('.+'))",
           String(["abcdefghijklmnopqrstuvwxyz"]), String('abcdefghijklmnopqrstuvwxyz'.match(new RegExp('.+'))));

// 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.match(new RegExp('.+'))
Assert.expectEq (  "'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.match(new RegExp('.+'))",
           String(["ABCDEFGHIJKLMNOPQRSTUVWXYZ"]), String('ABCDEFGHIJKLMNOPQRSTUVWXYZ'.match(new RegExp('.+'))));

// '`1234567890-=~!@#$%^&*()_+'.match(new RegExp('.+'))
Assert.expectEq (  "'`1234567890-=~!@#$%^&*()_+'.match(new RegExp('.+'))",
           String(["`1234567890-=~!@#$%^&*()_+"]), String('`1234567890-=~!@#$%^&*()_+'.match(new RegExp('.+'))));

// '|\\[{]};:"\',<>.?/'.match(new RegExp('.+'))
Assert.expectEq (  "'|\\[{]};:\"\',<>.?/'.match(new RegExp('.+'))",
           String(["|\\[{]};:\"\',<>.?/"]), String('|\\[{]};:\"\',<>.?/'.match(new RegExp('.+'))));

// '|\\[{]};:"\',<>.?/'.match(/.+/)
Assert.expectEq (  "'|\\[{]};:\"\',<>.?/'.match(/.+/)",
           String(["|\\[{]};:\"\',<>.?/"]), String('|\\[{]};:\"\',<>.?/'.match(/.+/)));

