/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'beginLine.js';

/**
   Filename:     beginLine.js
   Description:  'Tests regular expressions containing ^'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'RegExp: ^';



// 'abcde'.match(new RegExp('^ab'))
Assert.expectEq (  "'abcde'.match(new RegExp('^ab'))",
           String(["ab"]), String('abcde'.match(new RegExp('^ab'))));

// 'ab\ncde'.match(new RegExp('^..^e'))
Assert.expectEq (  "'ab\ncde'.match(new RegExp('^..^e'))",
           null, 'ab\ncde'.match(new RegExp('^..^e')));

// 'yyyyy'.match(new RegExp('^xxx'))
Assert.expectEq (  "'yyyyy'.match(new RegExp('^xxx'))",
           null, 'yyyyy'.match(new RegExp('^xxx')));

// '^^^x'.match(new RegExp('^\\^+'))
Assert.expectEq (  "'^^^x'.match(new RegExp('^\\^+'))",
           String(['^^^']), String('^^^x'.match(new RegExp('^\\^+'))));

// '^^^x'.match(/^\^+/)
Assert.expectEq (  "'^^^x'.match(/^\\^+/)",
           String(['^^^']), String('^^^x'.match(/^\^+/)));


// RegExp.multiline = true;   <-- readonly property in AS3, must use m (multiline) flag

// 'abc\n123xyz'.match(new RegExp('^\d+')) <multiline==true>
Assert.expectEq (  "'abc\n123xyz'.match(new RegExp('^\\d+', 'm'))",
           String(['123']), String('abc\n123xyz'.match(new RegExp('^\\d+', 'm'))));

