/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'backslash.js';

/**
   Filename:     backslash.js
   Description:  'Tests regular expressions containing \'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'RegExp: \\';


// 'abcde'.match(new RegExp('\e'))
Assert.expectEq (  "'abcde'.match(new RegExp('\e'))",
           String(["e"]), String('abcde'.match(new RegExp('\e'))));

// 'ab\\cde'.match(new RegExp('\\\\'))
Assert.expectEq (  "'ab\\cde'.match(new RegExp('\\\\'))",
           String(["\\"]), String('ab\\cde'.match(new RegExp('\\\\'))));

// 'ab\\cde'.match(/\\/) (using literal)
Assert.expectEq (  "'ab\\cde'.match(/\\\\/)",
           String(["\\"]), String('ab\\cde'.match(/\\/)));

// 'before ^$*+?.()|{}[] after'.match(new RegExp('\^\$\*\+\?\.\(\)\|\{\}\[\]'))
Assert.expectEq ( 
           "'before ^$*+?.()|{}[] after'.match(new RegExp('\\^\\$\\*\\+\\?\\.\\(\\)\\|\\{\\}\\[\\]'))",
           String(["^$*+?.()|{}[]"]),
           String('before ^$*+?.()|{}[] after'.match(new RegExp('\\^\\$\\*\\+\\?\\.\\(\\)\\|\\{\\}\\[\\]'))));

// 'before ^$*+?.()|{}[] after'.match(/\^\$\*\+\?\.\(\)\|\{\}\[\]/) (using literal)
Assert.expectEq ( 
           "'before ^$*+?.()|{}[] after'.match(/\\^\\$\\*\\+\\?\\.\\(\\)\\|\\{\\}\\[\\]/)",
           String(["^$*+?.()|{}[]"]),
           String('before ^$*+?.()|{}[] after'.match(/\^\$\*\+\?\.\(\)\|\{\}\[\]/)));

