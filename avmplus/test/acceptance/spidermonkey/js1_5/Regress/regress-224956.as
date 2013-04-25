/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 *
 * Date:    08 November 2003
 * SUMMARY: |expr()| should cause a TypeError if |typeof expr| != 'function'
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=224956
 *
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-224956.js';
var UBound = 0;
var BUGNUMBER = 224956;
var summary = "|expr()| should cause TypeError if |typeof expr| != 'function'";
var TEST_PASSED = 'TypeError';
var TEST_FAILED = 'Generated an error, but NOT a TypeError! ';
var TEST_FAILED_BADLY = 'Did not generate ANY error!!!';
var CHECK_PASSED = 'Should not generate an error';
var CHECK_FAILED = 'Generated an error!';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];
var x;


/*
 * All the following contain invalid uses of the call operator ()
 * and should generate TypeErrors. That's what we're testing for.
 *
 * To save writing try...catch over and over, we hide all the
 * errors inside eval strings, and let testThis() catch them.
 */
//status = inSection(1);

testThis(function () {x = 'abc'; x()});

//status = inSection(2);
testThis(function () {"abc"()});

//status = inSection(3);
testThis(function () {x = new Date(); x()});

//status = inSection(4);
testThis(function () {Date(12345)()});

//status = inSection(5);
testThis(function () {x = Number(1); x()});

//status = inSection(6);
testThis(function () {1()});

//status = inSection(7);
testThis(function () {x = void(0); x()});

//status = inSection(8);
testThis(function () {void(0)()});

//status = inSection(9);
testThis(function () {x = Math; x()});

//status = inSection(10);
testThis(function () {Math()});

//status = inSection(11);
testThis(function () {x = Array(5); x()});

//status = inSection(12);
testThis(function () {x=[1,2,3,4,5]; x()});

//status = inSection(13);
testThis(function () {x = [1,2,3].splice(1,2); x()});


/*
 * Same as above, but with non-empty call parentheses
 */
//status = inSection(14);
testThis(function () {x = 'abc'; x(1)});

//status = inSection(15);
testThis(function () {"abc"(1)});

//status = inSection(16);
testThis(function () {x = new Date(); x(1)});

//status = inSection(17);
testThis(function () {Date(12345)(1)});

//status = inSection(18);
testThis(function () {x = Number(1); x(1)});

//status = inSection(19);
testThis(function () {1(1)});

//status = inSection(20);
testThis(function () {x = void(0); x(1)});

//status = inSection(21);
testThis(function () {void(0)(1)});

//status = inSection(22);
testThis(function () {x = Math; x(1)});

//status = inSection(23);
testThis(function () {Math(1)});

//status = inSection(24);
testThis(function () {x = Array(5); x(1)});

//status = inSection(25);
testThis(function () {x=[1,2,3,4,5]; x(1)});

//status = inSection(26);
testThis(function () {x = [1,2,3].splice(1,2); x(1)});


/*
 * Expression from website in original bug report above -
 */
//status = inSection(27);
var A = 1, C=2, Y=3, T=4, I=5;
testThis(function () {(((C/A-0.3)/0.2)+((Y/A-3)/4)+((T/A)/0.05)+((0.095-I/A)/0.4))(100/6)});



/*
 * Functions and RegExps both have |typeof| == 'function'.
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=61911.
 *
 * Therefore these expressions should not cause any errors.
 * Note we use checkThis() instead of testThis()
 *
 */
//status = inSection(28);
checkThis(function () {x = function (y) {return y+1;}; x("abc")});

//status = inSection(29);
checkThis(function () {(function (y) {return y+1;})("abc")});

//status = inSection(30);

checkThis(function () {function f(y) { function g() {return y;}; return g();}; f("abc")});

//status = inSection(31);

checkThis(function () {x = /a()/; x("abc")});

//status = inSection(32);
checkThis(function () {x = /a()/gi; x("abc")});

//status = inSection(33);
checkThis(function () {x = RegExp('a()'); x("abc")});

//status = inSection(34);
checkThis(function () {x = new RegExp('a()', 'gi'); x("")});




//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------




/*
 * |expr()| should generate a TypeError if |expr| is not a function
 */
function testThis(testFunc)
{
  expect = TEST_PASSED;
  actual = TEST_FAILED_BADLY;

  try
  {
    testFunc();
  }
  catch(e)
  {
    if (e instanceof TypeError)
      actual = TEST_PASSED;
    else
      actual = TEST_FAILED + e;
  }

  statusitems[UBound] = status;
  expectedvalues[UBound] = expect;
  actualvalues[UBound] = actual;
  UBound++;
}


/*
 * Valid syntax shouldn't generate any errors
 */
function checkThis(testFunc)
{
  expect = CHECK_PASSED;
  actual = CHECK_PASSED;

  try
  {
    testFunc();
  }
  catch(e)
  {
    actual = CHECK_FAILED;
  }

  statusitems[UBound] = status;
  expectedvalues[UBound] = expect;
  actualvalues[UBound] = actual;
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

