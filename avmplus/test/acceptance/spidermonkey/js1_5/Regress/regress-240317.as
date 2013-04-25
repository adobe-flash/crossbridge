/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 * Contributor: Bob Clary
 */
import com.adobe.test.Assert;

var gTestfile = 'regress-240317.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 240317;
var summary = 'Using Reserved identifiers warns';
var actual = '';
var expect = 'no error';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

function testvar(words)
{
  var e;
  expect = 'no error';
  for (var i = 0; i < words.length; i++)
  {
    var word = words[i];

    actual = '';
    status = summary + ': ' + word;
    try
    {
      this[word];
      actual = 'no error';
    }
    catch(e)
    {
      actual = 'error';
      status +=  ', ' + e.name + ': ' + e.message + ' ';
    }
    Assert.expectEq(summary, expect, actual);

    actual = '';
    status = summary + ': ' + word;
    try
    {
      this[word] = "foo";
      actual = 'no error';
    }
    catch(e)
    {
      actual = 'error';
      status +=  ', ' + e.name + ': ' + e.message + ' ';
    }
    Assert.expectEq(summary, expect, actual);

  }
}

// future reserved words
var reserved =
  ['abstract',    'enum',      'int',      'short',      'boolean',
   'interface', 'static',   'byte',       'extends',
   'long',         'super',     'char',     'final',      'native',
   'synchronized', 'class',     'float',    'package',    'throws',
   'goto',      'private',  'transient',
   'implements',   'protected', 'volatile', 'double',
   'public'];

testvar(reserved);
 


