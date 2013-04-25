/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'RegExp_dollar_number.js';

/**
   Filename:     RegExp_dollar_number.js
   Description:  'Tests RegExps $1, ..., $9 properties'

   Author:       Nick Lerissa
   Date:         March 12, 1998
*/

// var SECTION = 'As described in Netscape doc "What\'s new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'RegExp: $1, ..., $9';
var BUGNUMBER="123802";



// 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); RegExp.$1
var result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/);
Assert.expectEq (  "result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); result[1]",
           'abcdefghi', result[1]);

// 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); RegExp.$2
Assert.expectEq (  "result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); result[2]",
           'bcdefgh', result[2]);

// 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); RegExp.$3
Assert.expectEq (  "result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); result[3]",
           'cdefg', result[3]);

// 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); RegExp.$4
Assert.expectEq (  "result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); result[4]",
           'def', result[4]);

// 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); RegExp.$5
Assert.expectEq (  "result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); result[5]",
           'e', result[5]);

// 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); RegExp.$6
Assert.expectEq (  "result = 'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/); result[6]",
           undefined, result[6]);

var a_to_z = 'abcdefghijklmnopqrstuvwxyz';
var regexp1 = /(a)b(c)d(e)f(g)h(i)j(k)l(m)n(o)p(q)r(s)t(u)v(w)x(y)z/
  // 'abcdefghijklmnopqrstuvwxyz'.match(/(a)b(c)d(e)f(g)h(i)j(k)l(m)n(o)p(q)r(s)t(u)v(w)x(y)z/); RegExp.$1
var result = a_to_z.match(regexp1);

Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[1]",
           'a', result[1]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[2]",
           'c', result[2]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[3]",
           'e', result[3]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[4]",
           'g', result[4]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[5]",
           'i', result[5]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[6]",
           'k', result[6]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[7]",
           'm', result[7]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[8]",
           'o', result[8]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[9]",
           'q', result[9]);
Assert.expectEq (  "'" + a_to_z + "'.match((a)b(c)....(y)z); result[10]",
           's', result[10]);

