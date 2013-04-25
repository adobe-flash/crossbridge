/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;


/*
 *
 * Date:    11 Nov 2002
 * SUMMARY: JS shouldn't crash on extraneous args to str.match(), etc.
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=179524
 *
 * Note that when testing str.replace(), we have to be careful if the first
 * argument provided to str.replace() is not a regexp object. ECMA-262 says
 * it is NOT converted to one, unlike the case for str.match(), str.search().
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=83293#c21. This means
 * we have to be careful how we test meta-characters in the first argument
 * to str.replace(), if that argument is a string -
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-179524.js';
var UBound = 0;
var BUGNUMBER = 179524;
var summary = "Don't crash on extraneous arguments to str.match(), etc.";
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


str = 'ABC abc';
var re = /z/ig;

//status = inSection(1);
actual = str.match(re);
expect = null;
print(actual.length)
Assert.expectEq(summary, expect, actual);

//status = inSection(2);
actual = function () {str.match(re, 'i')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(3);
actual = function () {str.match(re, 'g', '')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(4);
actual = function () {str.match(re, 'z', new Object(), new Date())};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);


/*
 * Now try the same thing with str.search()
 */
//status = inSection(5);
actual = str.search(re);
expect = -1;
addThis();

//status = inSection(6);
actual = function () {str.search(re, 'i')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(7);
actual = function () {str.search(re, 'g', '')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(8);
actual = function () {str.search(re, 'z', new Object(), new Date())};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);


/*
 * Now try the same thing with str.replace()
 */
//status = inSection(9);
actual = str.replace(re, 'Z');
expect = str;
addThis();

//status = inSection(10);
actual = function () {str.replace(re, 'Z', 'i')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(11);
actual = function () {str.replace(re, 'Z', 'g', '')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(12);
actual = function () {str.replace(re, 'Z', 'z', new Object(), new Date())};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);



/*
 * Now test the case where str.match()'s first argument is not a regexp object.
 * In that case, JS follows ECMA-262 Ed.3 by converting the 1st argument to a
 * regexp object using the argument as a regexp pattern, but then extends ECMA
 * by taking any optional 2nd argument to be a regexp flag string (e.g.'ig').
 *
 * Reference: http://bugzilla.mozilla.org/show_bug.cgi?id=179524#c10
 */
//status = inSection(13);
actual = str.match('a').toString();
expect = str.match(/a/).toString();
addThis();

//status = inSection(14);
actual = str.match(new RegExp('a', 'i')).toString();
expect = str.match(/a/i).toString();
addThis();

//status = inSection(15);
actual = str.match(new RegExp('a', 'ig')).toString();
expect = str.match(/a/ig).toString();
addThis();

//status = inSection(16);
actual = str.match(new RegExp('\\s', 'm')).toString();
expect = str.match(/\s/m).toString();
addThis();


/*
 * Now try the previous three cases with extraneous parameters
 */
//status = inSection(17);
actual = function () {str.match(new RegExp('a', 'i'), 'g').toString()};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(18);
actual = function () {str.match(new RegExp('a', 'ig'), new Object()).toString()};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(19);
actual = function () {str.match(new RegExp('\\s', 'm'), 999).toString()};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);


/*
 * Try an invalid second parameter (i.e. an invalid regexp flag)
 */
//status = inSection(20);
try
{
  actual = str.match(new RegExp('a', 'z')).toString();
  expect = 'SHOULD HAVE FALLEN INTO CATCH-BLOCK!';
  addThis();
}
catch (e)
{
  actual = e instanceof SyntaxError;
  expect = true;
  addThis();
}



/*
 * Now test str.search() where the first argument is not a regexp object.
 * The same considerations as above apply -
 *
 * Reference: http://bugzilla.mozilla.org/show_bug.cgi?id=179524#c16
 */
//status = inSection(21);
actual = str.search('a');
expect = str.search(/a/);
addThis();

//status = inSection(22);
actual = str.search(new RegExp('a', 'i'));
expect = str.search(/a/i);
addThis();

//status = inSection(23);
actual = str.search(new RegExp('a', 'ig'));
expect = str.search(/a/ig);
addThis();

//status = inSection(24);
actual = str.search(new RegExp('\\s', 'm'));
expect = str.search(/\s/m);
addThis();


/*
 * Now try the previous three cases with extraneous parameters
 */
//status = inSection(25);
actual = function () {str.search(new RegExp('a', 'i'), 'g')};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(26);
actual = function () {str.search(new RegExp('a', 'ig'), new Object())};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);

//status = inSection(27);
actual = function () {str.search(new RegExp('\\s', 'm'), 999)};
expect = 'ArgumentError: Error #1063';
Assert.expectError(status, expect, actual);


/*
 * Try an invalid second parameter (i.e. an invalid regexp flag)
 */
//status = inSection(28);
try
{
  actual = str.search(new RegExp('a', 'z'));
  expect = 'SHOULD HAVE FALLEN INTO CATCH-BLOCK!';
  addThis();
}
catch (e)
{
  actual = e instanceof SyntaxError;
  expect = true;
  addThis();
}



/*
 * Now test str.replace() where the first argument is not a regexp object.
 * The same considerations as above apply, EXCEPT for meta-characters.
 * See introduction to testcase above. References:
 *
 * http://bugzilla.mozilla.org/show_bug.cgi?id=179524#c16
 * http://bugzilla.mozilla.org/show_bug.cgi?id=83293#c21
 */
//status = inSection(29);
actual = str.replace('a', 'Z');
expect = str.replace(/a/, 'Z');
addThis();

/*
//status = inSection(30);
actual = str.replace('a', 'Z', 'i');
expect = str.replace(/a/i, 'Z');
addThis();

//status = inSection(31);
actual = str.replace('a', 'Z', 'ig');
expect = str.replace(/a/ig, 'Z');
addThis();

//status = inSection(32);
actual = str.replace('\\s', 'Z', 'm'); //<--- NO!!! No meta-characters 1st arg!
actual = str.replace(' ', 'Z', 'm');   //<--- Have to do this instead
expect = str.replace(/\s/m, 'Z');
addThis();
*/

/*
 * Now try the previous three cases with extraneous parameters
 */
/*
//status = inSection(33);
actual = str.replace('a', 'Z', 'i', 'g');
expect = str.replace(/a/i, 'Z');
addThis();

//status = inSection(34);
actual = str.replace('a', 'Z', 'ig', new Object());
expect = str.replace(/a/ig, 'Z');
addThis();

//status = inSection(35);
actual = str.replace('\\s', 'Z', 'm', 999); //<--- NO meta-characters 1st arg!
actual = str.replace(' ', 'Z', 'm', 999);   //<--- Have to do this instead
expect = str.replace(/\s/m, 'Z');
addThis();
*/

/*
 * Try an invalid third parameter (i.e. an invalid regexp flag)
 */
//status = inSection(36);
try
{
  actual = str.replace(new RegExp('a', 'Z'), 'z');
  expect = 'SHOULD HAVE FALLEN INTO CATCH-BLOCK!';
  addThis();
}
catch (e)
{
  actual = e instanceof SyntaxError;
  expect = true;
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

