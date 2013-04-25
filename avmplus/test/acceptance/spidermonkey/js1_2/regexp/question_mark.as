/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'question_mark.js';

/**
   Filename:     question_mark.js
   Description:  'Tests regular expressions containing ?'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: ?';


// 'abcdef'.match(new RegExp('cd?e'))
Assert.expectEq (  "'abcdef'.match(new RegExp('cd?e'))",
           String(["cde"]), String('abcdef'.match(new RegExp('cd?e'))));

// 'abcdef'.match(new RegExp('cdx?e'))
Assert.expectEq (  "'abcdef'.match(new RegExp('cdx?e'))",
           String(["cde"]), String('abcdef'.match(new RegExp('cdx?e'))));

// 'pqrstuvw'.match(new RegExp('o?pqrst'))
Assert.expectEq (  "'pqrstuvw'.match(new RegExp('o?pqrst'))",
           String(["pqrst"]), String('pqrstuvw'.match(new RegExp('o?pqrst'))));

// 'abcd'.match(new RegExp('x?y?z?'))
Assert.expectEq (  "'abcd'.match(new RegExp('x?y?z?'))",
           String([""]), String('abcd'.match(new RegExp('x?y?z?'))));

// 'abcd'.match(new RegExp('x?ay?bz?c'))
Assert.expectEq (  "'abcd'.match(new RegExp('x?ay?bz?c'))",
           String(["abc"]), String('abcd'.match(new RegExp('x?ay?bz?c'))));

// 'abcd'.match(/x?ay?bz?c/)
Assert.expectEq (  "'abcd'.match(/x?ay?bz?c/)",
           String(["abc"]), String('abcd'.match(/x?ay?bz?c/)));

// 'abbbbc'.match(new RegExp('b?b?b?b'))
Assert.expectEq (  "'abbbbc'.match(new RegExp('b?b?b?b'))",
           String(["bbbb"]), String('abbbbc'.match(new RegExp('b?b?b?b'))));

// '123az789'.match(new RegExp('ab?c?d?x?y?z'))
Assert.expectEq (  "'123az789'.match(new RegExp('ab?c?d?x?y?z'))",
           String(["az"]), String('123az789'.match(new RegExp('ab?c?d?x?y?z'))));

// '123az789'.match(/ab?c?d?x?y?z/)
Assert.expectEq (  "'123az789'.match(/ab?c?d?x?y?z/)",
           String(["az"]), String('123az789'.match(/ab?c?d?x?y?z/)));

// '?????'.match(new RegExp('\\??\\??\\??\\??\\??'))
Assert.expectEq (  "'?????'.match(new RegExp('\\??\\??\\??\\??\\??'))",
           String(["?????"]), String('?????'.match(new RegExp('\\??\\??\\??\\??\\??'))));

// 'test'.match(new RegExp('.?.?.?.?.?.?.?'))
Assert.expectEq (  "'test'.match(new RegExp('.?.?.?.?.?.?.?'))",
           String(["test"]), String('test'.match(new RegExp('.?.?.?.?.?.?.?'))));

