/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'endLine.js';

/**
   Filename:     endLine.js
   Description:  'Tests regular expressions containing $'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: $';



// 'abcde'.match(new RegExp('de$'))
Assert.expectEq (  "'abcde'.match(new RegExp('de$'))",
           String(["de"]), String('abcde'.match(new RegExp('de$'))));

// 'ab\ncde'.match(new RegExp('..$e$'))
Assert.expectEq (  "'ab\ncde'.match(new RegExp('..$e$'))",
           null, 'ab\ncde'.match(new RegExp('..$e$')));

// 'yyyyy'.match(new RegExp('xxx$'))
Assert.expectEq (  "'yyyyy'.match(new RegExp('xxx$'))",
           null, 'yyyyy'.match(new RegExp('xxx$')));

// 'a$$$'.match(new RegExp('\\$+$'))
Assert.expectEq (  "'a$$$'.match(new RegExp('\\$+$'))",
           String(['$$$']), String('a$$$'.match(new RegExp('\\$+$'))));

// 'a$$$'.match(/\$+$/)
Assert.expectEq (  "'a$$$'.match(/\\$+$/)",
           String(['$$$']), String('a$$$'.match(/\$+$/)));

//RegExp.multiline = true;
// 'abc\n123xyz890\nxyz'.match(new RegExp('\d+$')) <multiline==true>
Assert.expectEq (  "'abc\n123xyz890\nxyz'.match(new RegExp('\\d+$', 'm'))",
           String(['890']), String('abc\n123xyz890\nxyz'.match(new RegExp('\\d+$', 'm'))));

