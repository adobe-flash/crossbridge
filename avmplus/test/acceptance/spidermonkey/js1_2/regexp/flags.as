/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'flags.js';

/**
   Filename:     regexp.js
   Description:  'Tests regular expressions using flags "i" and "g"'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE   = 'regular expression flags with flags "i" and "g"';



// testing optional flag 'i'
Assert.expectEq (  "'aBCdEfGHijKLmno'.match(/fghijk/i)",
           String(["fGHijK"]), String('aBCdEfGHijKLmno'.match(/fghijk/i)));

Assert.expectEq (  "'aBCdEfGHijKLmno'.match(new RegExp('fghijk','i'))",
           String(["fGHijK"]), String('aBCdEfGHijKLmno'.match(new RegExp("fghijk","i"))));

// testing optional flag 'g'
Assert.expectEq (  "'xa xb xc xd xe xf'.match(/x./g)",
           String(["xa","xb","xc","xd","xe","xf"]), String('xa xb xc xd xe xf'.match(/x./g)));

Assert.expectEq (  "'xa xb xc xd xe xf'.match(new RegExp('x.','g'))",
           String(["xa","xb","xc","xd","xe","xf"]), String('xa xb xc xd xe xf'.match(new RegExp('x.','g'))));

// testing optional flags 'g' and 'i'
Assert.expectEq (  "'xa Xb xc xd Xe xf'.match(/x./gi)",
           String(["xa","Xb","xc","xd","Xe","xf"]), String('xa Xb xc xd Xe xf'.match(/x./gi)));

Assert.expectEq (  "'xa Xb xc xd Xe xf'.match(new RegExp('x.','gi'))",
           String(["xa","Xb","xc","xd","Xe","xf"]), String('xa Xb xc xd Xe xf'.match(new RegExp('x.','gi'))));

Assert.expectEq (  "'xa Xb xc xd Xe xf'.match(/x./ig)",
           String(["xa","Xb","xc","xd","Xe","xf"]), String('xa Xb xc xd Xe xf'.match(/x./ig)));

Assert.expectEq (  "'xa Xb xc xd Xe xf'.match(new RegExp('x.','ig'))",
           String(["xa","Xb","xc","xd","Xe","xf"]), String('xa Xb xc xd Xe xf'.match(new RegExp('x.','ig'))));



