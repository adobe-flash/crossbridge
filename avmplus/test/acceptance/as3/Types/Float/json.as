/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
Returns an implementation-dependent approximation to the arc tangent of x.
The result is expressed in radians and ranges from iPI/2 to +PI/2.
*/

// var SECTION = "XXX";
// var VERSION = "AS3";
// var TITLE   = "JSON functionality operating on float";


// Testing the following on float:
//
//  - float data are formatted as Number
//  - toJSON on float.prototype is obeyed
//  - passing in float values for the number of spaces in stringify works
//  - using float values as property names in the replacer array works
//
// The two tests for "float values" are necessary because the JSON code switches on type
// and there is no generic "numeric" type for it to test, it proceeds by cases.

Assert.expectEq("float serializes as number", "37.5", JSON.stringify(37.5f));

Assert.expectEq("float can be used to denote spaces", "[\n    37\n]", JSON.stringify([37], null, 4f));

Assert.expectEq("float can be used as a property name in the replacer array", "{\"1\":\"b\",\"3\":\"d\"}", JSON.stringify({0:"a",1:"b",2:"c",3:"d"},[1f,3f]));

float.prototype.toJSON = float.prototype.toJSON = function (k) { return this + 5};
Assert.expectEq("float's toJSON is invoked properly and its value used", "[6,7,3]", JSON.stringify([1f,2f,3])); // Note, "3" is not intended to be float
delete float.prototype.toJSON;

