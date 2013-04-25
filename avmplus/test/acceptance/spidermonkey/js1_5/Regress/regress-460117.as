/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-460117.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 460117;
var summary = 'TM: hasOwnProperty with JIT';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  function t(o, proplist) {
    var props=proplist.split(/\s+/g);
    for (var i=0, len=props.length; i<len; i++) {
      if (o.hasOwnProperty(props[i])) {
        // do something
      } else {
        actual += (props[i]+': '+o.hasOwnProperty(props[i]));
      }
    }
  };

  t({ bar: 123, baz: 123, quux: 123 }, 'bar baz quux');

  Assert.expectEq(summary + ' : nonjit', expect, actual);



  t({ bar: 123, baz: 123, quux: 123 }, 'bar baz quux');



  Assert.expectEq(summary + ' : jit', expect, actual);


}

