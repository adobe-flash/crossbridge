/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-469044.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 469044;
var summary = 'type unstable globals';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  expect = '---000---000';
  actual = '';

  for (var i = 0; i < 2; ++i) {
    for (var e = 0; e < 2; ++e) {
    }
    var c = void 0;
    print(actual += "---");
    for (var a = 0; a < 3; ++a) {
      c <<= c;
      print(actual += "" + c);
    }
  }
  Assert.expectEq(summary + ': 1', expect, actual);

  expect = '00000000';
  actual = '';

  print("");
  for (var i = 0; i < 2; ++i) {
    for (var e = 0; e < 2; ++e) {
    }
    var c = void 0;
    for (var a = 0; a < 3; ++a) {
      c <<= c;
      print(actual += "" + c);
    }
    print(actual += c);
  }
  Assert.expectEq(summary + ': 2', expect, actual);

  actual = '';
  print("");

  for (var i = 0; i < 2; ++i) {
    for (var e = 0; e < 2; ++e) {
    }
    var c = void 0;
    for (var a = 0; a < 3; ++a) {
      c <<= c;
      Math;
      print(actual += "" + c);
    }
    print(actual += c);
  }
  Assert.expectEq(summary + ': 3', expect, actual);


}

