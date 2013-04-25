/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 *
 * Date:    16 Mar 2002
 * SUMMARY: Shouldn't crash if define |var arguments| inside a function
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=131510
 *
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-131510-001.js';
var BUGNUMBER = 131510;
var summary = "Shouldn't crash if define |var arguments| inside a function";
//printBugNumber(BUGNUMBER);
//printStatus(summary);


function f() {var arguments;}
f();


/*
 * Put same example in function scope instead of global scope
 */
function g() { function f() {var arguments;}; f();};
g();

function f() {var arguments;}; f();;

function g() { function f() {var arguments;}; f();}; g();

Assert.expectEq('', 'No Crash', 'No Crash');

