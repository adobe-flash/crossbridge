/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-364104.js';
//-----------------------------------------------------------------------------
var BUGNUMBER     = "364104";
var summary = "Array.prototype.indexOf, Array.prototype.lastIndexOf issues " +
  "with the optional second fromIndex argument";
var actual, expect;

//printBugNumber(BUGNUMBER);
//printStatus(summary);

/**************
 * BEGIN TEST *
 **************/

var failed = false;

Assert.expectEq('[2].indexOf(2) == 0', true, [2].indexOf(2) == 0);          // indexOf: not finding 2!?
Assert.expectEq('[2].indexOf(2, 0) == 0', true, [2].indexOf(2, 0) == 0);    // indexOf: not interpreting explicit second argument 0!
Assert.expectEq('[2].indexOf(2, 1) == -1', true, [2].indexOf(2, 1) == -1);  // indexOf: ignoring second argument with value equal to array length!
Assert.expectEq('[2].indexOf(2, 2) == -1', true, [2].indexOf(2, 2) == -1);  // indexOf: ignoring second argument greater than array length!
Assert.expectEq('[2].indexOf(2, 17) == -1', true, [2].indexOf(2, 17) == -1);// indexOf: ignoring large second argument!
Assert.expectEq('[2].indexOf(2, -5) == 0', true, [2].indexOf(2, -5) == 0);  // indexOf: calculated fromIndex < 0, should search entire array!
Assert.expectEq('[2, 3].indexOf(2, -1) == -1', true, [2, 3].indexOf(2, -1) == -1);  // indexOf: not handling index == (-1 + 2), element 2 correctly!
Assert.expectEq('[2, 3].indexOf(3, -1) == 1', true, [2, 3].indexOf(3, -1) == 1);    // indexOf: not handling index == (-1 + 2), element 3 correctly!
Assert.expectEq('[2].lastIndexOf(2) == 0', true, [2].lastIndexOf(2) == 0);          // lastIndexOf: not finding 2!?
Assert.expectEq('[2].lastIndexOf(2, 1) == 0', true, [2].lastIndexOf(2, 1) == 0);    // lastIndexOf: not interpreting explicit second argument 1!?
Assert.expectEq('[2].lastIndexOf(2, 17) == 0', true, [2].lastIndexOf(2, 17) == 0);   // lastIndexOf: should have searched entire array!
Assert.expectEq('[2].lastIndexOf(2, -5) == -1', true, [2].lastIndexOf(2, -5) == -1); // lastIndexOf: -5 + 1 < 0, so array shouldn't be searched!
Assert.expectEq('[2].lastIndexOf(2, -2) == -1', true, [2].lastIndexOf(2, -2) == -1); // lastIndexOf: -2 + 1 < 0, so array shouldn't be searched!
Assert.expectEq('[2, 3].lastIndexOf(2, -1) == 0', true, [2, 3].lastIndexOf(2, -1) == 0); // lastIndexOf: not handling index == (-1 + 2), element 2 correctly!
Assert.expectEq('[2, 3].lastIndexOf(3, -1) == 1', true, [2, 3].lastIndexOf(3, -1) == 1); // lastIndexOf: not handling index == (-1 + 2), element 3 correctly!
Assert.expectEq('[2, 3].lastIndexOf(2, -2) == 0', true, [2, 3].lastIndexOf(2, -2) == 0); // lastIndexOf: not handling index == (-2 + 2), element 2 correctly!
Assert.expectEq('[2, 3].lastIndexOf(3, -2) == -1', true, [2, 3].lastIndexOf(3, -2) == -1); // lastIndexOf: not handling index == (-2 + 2), element 3 correctly!
Assert.expectEq('[2, 3].lastIndexOf(2, -3) == -1', true, [2, 3].lastIndexOf(2, -3) == -1); // lastIndexOf: calculated fromIndex < 0, shouldn't search array for 2!
Assert.expectEq('[2, 3].lastIndexOf(3, -3) == -1', true, [2, 3].lastIndexOf(3, -3) == -1); // lastIndexOf: calculated fromIndex < 0, shouldn't search array for 3!

