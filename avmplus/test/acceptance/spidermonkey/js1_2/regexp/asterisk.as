/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'asterisk.js';

/**
   Filename:     asterisk.js
   Description:  'Tests regular expressions containing *'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: *';


// 'abcddddefg'.match(new RegExp('d*'))
Assert.expectEq (  "'abcddddefg'.match(new RegExp('d*'))",
           String([""]), String('abcddddefg'.match(new RegExp('d*'))));

// 'abcddddefg'.match(new RegExp('cd*'))
Assert.expectEq (  "'abcddddefg'.match(new RegExp('cd*'))",
           String(["cdddd"]), String('abcddddefg'.match(new RegExp('cd*'))));

// 'abcdefg'.match(new RegExp('cx*d'))
Assert.expectEq (  "'abcdefg'.match(new RegExp('cx*d'))",
           String(["cd"]), String('abcdefg'.match(new RegExp('cx*d'))));

// 'xxxxxxx'.match(new RegExp('(x*)(x+)'))
Assert.expectEq (  "'xxxxxxx'.match(new RegExp('(x*)(x+)'))",
           String(["xxxxxxx","xxxxxx","x"]), String('xxxxxxx'.match(new RegExp('(x*)(x+)'))));

// '1234567890'.match(new RegExp('(\\d*)(\\d+)'))
Assert.expectEq (  "'1234567890'.match(new RegExp('(\\d*)(\\d+)'))",
           String(["1234567890","123456789","0"]),
           String('1234567890'.match(new RegExp('(\\d*)(\\d+)'))));

// '1234567890'.match(new RegExp('(\\d*)\\d(\\d+)'))
Assert.expectEq (  "'1234567890'.match(new RegExp('(\\d*)\\d(\\d+)'))",
           String(["1234567890","12345678","0"]),
           String('1234567890'.match(new RegExp('(\\d*)\\d(\\d+)'))));

// 'xxxxxxx'.match(new RegExp('(x+)(x*)'))
Assert.expectEq (  "'xxxxxxx'.match(new RegExp('(x+)(x*)'))",
           String(["xxxxxxx","xxxxxxx",""]), String('xxxxxxx'.match(new RegExp('(x+)(x*)'))));

// 'xxxxxxyyyyyy'.match(new RegExp('x*y+$'))
Assert.expectEq (  "'xxxxxxyyyyyy'.match(new RegExp('x*y+$'))",
           String(["xxxxxxyyyyyy"]), String('xxxxxxyyyyyy'.match(new RegExp('x*y+$'))));

// 'abcdef'.match(/[\d]*[\s]*bc./)
Assert.expectEq (  "'abcdef'.match(/[\\d]*[\\s]*bc./)",
           String(["bcd"]), String('abcdef'.match(/[\d]*[\s]*bc./)));

// 'abcdef'.match(/bc..[\d]*[\s]*/)
Assert.expectEq (  "'abcdef'.match(/bc..[\\d]*[\\s]*/)",
           String(["bcde"]), String('abcdef'.match(/bc..[\d]*[\s]*/)));

// 'a1b2c3'.match(/.*/)
Assert.expectEq (  "'a1b2c3'.match(/.*/)",
           String(["a1b2c3"]), String('a1b2c3'.match(/.*/)));

// 'a0.b2.c3'.match(/[xyz]*1/)
Assert.expectEq (  "'a0.b2.c3'.match(/[xyz]*1/)",
           null, 'a0.b2.c3'.match(/[xyz]*1/));

