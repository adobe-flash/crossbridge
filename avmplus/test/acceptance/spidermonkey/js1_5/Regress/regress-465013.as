/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465013.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465013;
var summary = '';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
 
  expect = ',allowScriptAccess="dummy",bgcolor="dummy",quality="dummy"';
 


  (function(x) {
        var ja = "";
        var ka = {bgcolor:"#FFFFFF", quality:"high", allowScriptAccess:"always"};
        for (var la in ka) {
          ja +=[la] + "=\"" + x/*ka[la]*/ + "\" ";
        }
        return actual = ja;
      })("dummy");

    actual = actual.split(/\s+/).sort().toString();


  Assert.expectEq(summary, expect, actual);


}

