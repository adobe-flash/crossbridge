/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'ignoreCase.js';

/**
   Filename:     ignoreCase.js
   Description:  'Tests RegExp attribute ignoreCase'

   Author:       Nick Lerissa
   Date:         March 13, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'RegExp: ignoreCase';



// /xyz/i.ignoreCase
Assert.expectEq (  "/xyz/i.ignoreCase",
           true, /xyz/i.ignoreCase);

// /xyz/.ignoreCase
Assert.expectEq (  "/xyz/.ignoreCase",
           false, /xyz/.ignoreCase);

// 'ABC def ghi'.match(/[a-z]+/ig)
Assert.expectEq (  "'ABC def ghi'.match(/[a-z]+/ig)",
           String(["ABC","def","ghi"]), String('ABC def ghi'.match(/[a-z]+/ig)));

// 'ABC def ghi'.match(/[a-z]+/i)
Assert.expectEq (  "'ABC def ghi'.match(/[a-z]+/i)",
           String(["ABC"]), String('ABC def ghi'.match(/[a-z]+/i)));

// 'ABC def ghi'.match(/([a-z]+)/ig)
Assert.expectEq (  "'ABC def ghi'.match(/([a-z]+)/ig)",
           String(["ABC","def","ghi"]), String('ABC def ghi'.match(/([a-z]+)/ig)));

// 'ABC def ghi'.match(/([a-z]+)/i)
Assert.expectEq (  "'ABC def ghi'.match(/([a-z]+)/i)",
           String(["ABC","ABC"]), String('ABC def ghi'.match(/([a-z]+)/i)));

// 'ABC def ghi'.match(/[a-z]+/)
Assert.expectEq (  "'ABC def ghi'.match(/[a-z]+/)",
           String(["def"]), String('ABC def ghi'.match(/[a-z]+/)));

// (new RegExp('xyz','i')).ignoreCase
Assert.expectEq (  "(new RegExp('xyz','i')).ignoreCase",
           true, (new RegExp('xyz','i')).ignoreCase);

// (new RegExp('xyz')).ignoreCase
Assert.expectEq (  "(new RegExp('xyz')).ignoreCase",
           false, (new RegExp('xyz')).ignoreCase);

// 'ABC def ghi'.match(new RegExp('[a-z]+','ig'))
Assert.expectEq (  "'ABC def ghi'.match(new RegExp('[a-z]+','ig'))",
           String(["ABC","def","ghi"]), String('ABC def ghi'.match(new RegExp('[a-z]+','ig'))));

// 'ABC def ghi'.match(new RegExp('[a-z]+','i'))
Assert.expectEq (  "'ABC def ghi'.match(new RegExp('[a-z]+','i'))",
           String(["ABC"]), String('ABC def ghi'.match(new RegExp('[a-z]+','i'))));

// 'ABC def ghi'.match(new RegExp('([a-z]+)','ig'))
Assert.expectEq (  "'ABC def ghi'.match(new RegExp('([a-z]+)','ig'))",
           String(["ABC","def","ghi"]), String('ABC def ghi'.match(new RegExp('([a-z]+)','ig'))));

// 'ABC def ghi'.match(new RegExp('([a-z]+)','i'))
Assert.expectEq (  "'ABC def ghi'.match(new RegExp('([a-z]+)','i'))",
           String(["ABC","ABC"]), String('ABC def ghi'.match(new RegExp('([a-z]+)','i'))));

// 'ABC def ghi'.match(new RegExp('[a-z]+'))
Assert.expectEq (  "'ABC def ghi'.match(new RegExp('[a-z]+'))",
           String(["def"]), String('ABC def ghi'.match(new RegExp('[a-z]+'))));

