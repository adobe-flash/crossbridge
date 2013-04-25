/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 *
 * Date:    29 Oct 2002
 * SUMMARY: Testing "use" and "set" operations on expressions like a[i][j][k]
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=96526#c52
 *
 * Brendan: "The idea is to cover all the 'use' and 'set' (as in modify)
 * operations you can do on an expression like a[i][j][k], including variations
 * where you replace |a| with arguments (literally) and |i| with 0, 1, 2, etc.
 * (to hit the optimization for arguments[0]... that uses JSOP_ARGSUB)."
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-96526-delelem.js';
var UBound = 0;
var BUGNUMBER = 96526;
var summary = 'Testing "use" and "set" ops on expressions like a[i][j][k]';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


var CHAR_LBRACKET = '[';
var CHAR_RBRACKET = ']';
var CHAR_QT_DBL = '"';
var CHAR_QT = "'";
var CHAR_NL = '\n';
var CHAR_COMMA = ',';
var CHAR_SPACE = ' ';
var TYPE_STRING = typeof 'abc';

function formatArray(arr)
{
  try
  {
    return arr.toSource();
  }
  catch(e)
  {
    return toSource(arr);
  }
}

function toSource(arr)
{
  var delim = CHAR_COMMA + CHAR_SPACE;
  var elt = '';
  var ret = '';
  var len = arr.length;
  var i;
  for (i=0; i<len; i++)
  {
    elt = arr[i];

    switch(true)
    {
    case (typeof elt === TYPE_STRING) :
      ret += doubleQuote(elt);
      break;

    case (elt === undefined || elt === null) :
      break; // add nothing but the delimiter, below -

    default:
      ret += elt.toString();
    }

    if ((i < len-1) || (elt === undefined))
      ret += delim;
  }

  return  CHAR_LBRACKET + ret + CHAR_RBRACKET;
}


var z='magic';
Number.prototype.magic=42;
f(2,1,[-1,0,[1,2,[3,4]]]);

function f(j,k,a)
{
  //status = inSection(1);
  actual = formatArray(a[2]);
  expect = formatArray([1,2,[3,4]]);
  addThis();

  //status = inSection(2);
  actual = formatArray(a[2][j]);
  expect = formatArray([3,4]);
  addThis();

  //status = inSection(3);
  actual = a[2][j][k];
  expect = 4;
  addThis();

  //status = inSection(4);
  actual = a[2][j][k][z];
  expect = 42;
  addThis();

  delete a[2][j][k];

  //status = inSection(5);
  actual = formatArray(a[2][j]);
  expect = '[3, , ]';
  addThis();
}



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
//printStatus(summary);

  for (var i=0; i<UBound; i++)
  {
    Assert.expectEq(statusitems[i], expectedvalues[i], actualvalues[i]);
  }


}

