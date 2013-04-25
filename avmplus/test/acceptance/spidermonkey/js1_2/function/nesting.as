/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'nesting.js';

/**
   Filename:     nesting.js
   Description:  'This tests the nesting of functions'

   Author:       Nick Lerissa
   Date:         Fri Feb 13 09:58:28 PST 1998
*/

// var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
// var VERSION = 'no version';
// var TITLE = 'functions: nesting';


function outer_func(x)
{
  var y = "outer";

  Assert.expectEq(  "outer:x    ",
        1111,  x);
  Assert.expectEq(  "outer:y    ",
        'outer', y);
  function inner_func(x)
  {
    var y = "inner";
    Assert.expectEq(  "inner:x    ",
          2222,  x);
    Assert.expectEq(  "inner:y    ",
          'inner', y);
  };

  inner_func(2222);
  Assert.expectEq(  "outer:x    ",
        1111,  x);
  Assert.expectEq(  "outer:y    ",
        'outer', y);
}

outer_func(1111);


