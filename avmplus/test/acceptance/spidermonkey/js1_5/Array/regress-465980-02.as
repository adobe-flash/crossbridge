/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465980-02.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465980;
var summary = 'Do not crash @ InitArrayElements';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  function describe(name, startLength, pushArgs, expectThrow, expectLength)
  {
    return name + "(" + startLength + ", " +
      "[" + pushArgs.join(", ") + "], " +
      expectThrow + ", " +
      expectLength + ")";
  }

  function testArrayPush(startLength, pushArgs, expectThrow, expectLength)
  {
    /*
    print("running testArrayPush(" +
          startLength + ", " +
          "[" + pushArgs.join(", ") + "], " +
          expectThrow + ", " +
          expectLength + ")...");
    */
    var a = new Array(startLength);
    try
    {
      for (var i=0; i<pushArgs.length; i++) {
        a.push(pushArgs[i]);
      }
      if (expectThrow)
      {
        throw "expected to throw for " +
          describe("testArrayPush", startLength, pushArgs, expectThrow,
                   expectLength);
      }
    }
    catch (e)
    {
      if (!(e instanceof RangeError))
      {
        throw "unexpected exception type thrown: " + e + " for " +
          describe("testArrayPush", startLength, pushArgs, expectThrow,
                   expectLength);
      }
      if (!expectThrow)
      {
        throw "unexpected exception " + e + " for " +
          describe("testArrayPush", startLength, pushArgs, expectThrow,
                   expectLength);
      }
    }
    
    return a.length;
  }

  function testArrayUnshift(startLength, unshiftArgs, expectThrow, expectLength)
  {
    print("running testArrayUnshift(" +
          startLength + ", " +
          "[" + unshiftArgs.join(", ") + "], " +
          expectThrow + ", " +
          expectLength + ")...");
    
    print('initing array')
    var a = new Array(startLength);
    try
    {
      print('a.unshift(unshiftArgs);')
      a.unshift(unshiftArgs);
      if (expectThrow)
      {
        throw "expected to throw for " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
    }
    catch (e)
    {
      print('catch e: '+e.toString());
      if (!(e instanceof RangeError))
      {
        throw "unexpected exception type thrown: " + e + " for " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
      if (!expectThrow)
      {
        throw "unexpected exception " + e + " for " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
    }

    print('checklength');
    if (a.length !== expectLength)
    {
      throw "unexpected modified-array length for " +
        describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                 expectLength);
    }

    print('for')
    for (var i = 0, sz = unshiftArgs.length; i < sz; i++)
    {
      print('a[i] = '+a[i]+' unshiftArgs[i] = '+ unshiftArgs[i]);
      if (a[i] !== unshiftArgs[i])
      {
        throw "unexpected value at index " + index + " during " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
    }
  }




var foo = "foo", bar = "bar", baz = "baz";
    

try {
     actual = testArrayPush(4294967294, [foo, bar], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayPush(4294967294, [foo, bar], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967294, [foo, bar, baz], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayPush(4294967294, [foo, bar, baz], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967295, [foo], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayPush(4294967295, [foo], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967295, [foo, bar], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayPush(4294967295, [foo, bar], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967295, [foo, bar, baz], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayPush(4294967295, [foo, bar, baz], false, 4294967295)', 4294967295, actual);


/* unshift is very slow for large arrays
  
actual = ''

try {
     testArrayUnshift(4294967294, [foo], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayUnshift(4294967294, [foo], false, 4294967295)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967294, [foo, bar], true, 4294967294);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayUnshift(4294967294, [foo, bar], true, 4294967294)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967294, [foo, bar, baz], true, 4294967294);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayUnshift(4294967294, [foo, bar, baz], true, 4294967294)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967295, [foo], true, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayUnshift(4294967295, [foo], true, 4294967295)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967295, [foo, bar], true, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayUnshift(4294967295, [foo, bar], true, 4294967295)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967295, [foo, bar, baz], true, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 Assert.expectEq('testArrayUnshift(4294967295, [foo, bar, baz], true, 4294967295)', expect, actual);
*/

} // addtestcases

