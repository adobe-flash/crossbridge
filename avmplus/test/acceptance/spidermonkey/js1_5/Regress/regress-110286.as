/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 *
 * Date: 16 Nov 2001
 * SUMMARY: multiline comments containing "/*" should not be syntax errors
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=110286
 *
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-110286.js';
var UBound = 0;
var BUGNUMBER = 110286;
var summary = 'Multiline comments containing "/*" should not be syntax errors';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


//status = inSection(1);
actual = /* /* */3
expect = 3;
addThis();

//status = inSection(2);
actual = 3/* /* */
expect = 3;
addThis();

//status = inSection(3);
actual = /* 3/* */
expect = undefined;
addThis();

//status = inSection(4);
actual = /* /*3 */
expect = undefined;
addThis();

//status = inSection(5);
var passed = true;
try
{
  /* blah blah /* blah blah */
}
catch(e)
{
  passed = false;
}
actual = passed;
expect = true;
addThis();


//status = inSection(6);
try
{
  /*
    /*A/* /* /*A/*
    /* blah blah /*
    /* blah blah /*
    /* /*A/* /*A/*
  */
  var result = 'PASSED';
}
catch(e)
{
  var result = 'FAILED';
}
actual = result;
expect = 'PASSED';
addThis();


//status = inSection(7);
var str = 'ABC';
/*
 *    /*
 *    /*
 *    /*
 *    /*
 *
 */
str += 'DEF';
actual = str;
expect = 'ABCDEF';
addThis();



//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------


function addThis()
{
  statusitems[UBound] = status;
  actualvalues[UBound] = actual;
  expectedvalues[UBound] = expect;
  UBound++;
}


function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  for (var i = 0; i < UBound; i++)
  {
    Assert.expectEq(statusitems[i], expectedvalues[i], actualvalues[i]);
  }


}

