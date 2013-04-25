/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465377.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465377;
var summary = 'instanceof relations between Error objects';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  expect = actual = 'No Exception';

  try
  {
    var list = [
      Error,
      EvalError,
      RangeError,
      ReferenceError,
      SyntaxError,
      TypeError,
      URIError
      ];
    var instances = [];

    for (var i = 0; i != list.length; ++i) {
      instances[i] = new list[i]();
    }

    for (var i = 0; i != instances.length; ++i) {
      var instance = instances[i];
      var name = instance.name;
      var constructor = instance.constructor;
      if (!(instance instanceof Object))
        throw "Bad instanceof Object for "+name;
      if (!(instance instanceof Error))
        throw "Bad instanceof Error for "+name;
      if (!(instance instanceof constructor))
        throw "Bad instanceof constructor for "+name;
      if (instance instanceof Function)
        throw "Bad instanceof Function for "+name;
      for (var j = 1; j != instances.length; ++j) {
        if (i != j && instance instanceof instances[j].constructor) {
          throw "Unexpected (new "+name+") instanceof "+ instances[j].name;
        }
      }
    }

    print("OK");
  }
  catch(ex)
  {
    actual = ex + '';
  }
  Assert.expectEq(summary, expect, actual);


}

