/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-475645-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 475645;
var summary = 'Do not crash @ nanojit::LIns::isop';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  linkarr = new Array();
  picarr = new Array();
  textarr = new Array();
  var f=161;
  var t=27;
  var pics = "";
  var links = "";
  var texts = "";
  var s = f+t;
  var d = "1";
  picarr[2] = "2";
  for(i=1;i<picarr.length;i++)
  {
    if(pics=="") pics = picarr[i];
    else{
      if(picarr[i].indexOf("jpg")==-1 && picarr[i].indexOf("JPG")==-1) picarr[i] = d;
    }
  }



  Assert.expectEq(summary, expect, actual);


}

