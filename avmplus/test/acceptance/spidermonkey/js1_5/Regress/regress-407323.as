/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;


var gTestfile = 'regress-407323.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 407323;
var summary = 'XML, XMLList, QName are mutable, Namespace is not.';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  var obj           = {};
  var saveQName     = QName;
  var saveXML       = XML;
  var saveXMLList   = XMLList;
  var saveNamespace = Namespace;

  
  Assert.expectError('Attempt to assign to QName', 'ReferenceError: Error #1074', function () {QName = {}});
  Assert.expectError('Attempt to assign to XML', 'ReferenceError: Error #1074', function () {XML = {}});
  Assert.expectError('Attempt to assign to XMLList', 'ReferenceError: Error #1074', function () {XMLList = {}});
  Assert.expectError('Attempt to assign to Namespace', 'ReferenceError: Error #1074', function () {Namespace = {}});



}

