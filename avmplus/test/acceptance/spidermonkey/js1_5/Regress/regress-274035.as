/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-274035.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 274035;
var summary = 'Array.prototype[concat|slice|splice] lengths';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);
  
//status = summary + ' ' + inSection(1) + ' Array.prototype.concat.length ';
expect = 1;
actual   = Array.prototype.concat.length;
Assert.expectEq('Array.prototype.concat.length', expect, actual);
  
//status = summary + ' ' + inSection(2) + ' Array.prototype.slice.length ';
expect = 2;
actual   = Array.prototype.slice.length;
Assert.expectEq('Array.prototype.slice.length', expect, actual);
  
//status = summary + ' ' + inSection(3) + ' Array.prototype.splice.length ';
expect = 2;
actual   = Array.prototype.splice.length;
Assert.expectEq('Array.prototype.splice.length', expect, actual);
 

