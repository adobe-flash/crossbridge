/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-308806-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 308806;
var summary = 'Object.toLocaleString() should track Object.toString() ';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

var o = {toString: function() { return 'foo'; }};

expect = o.toString();
actual = o.toLocaleString();
 
Assert.expectEq(summary, expect, actual);

var a:Object = new Object();
a.toString = function () {return 'custom string'};
Assert.expectEq('Object.toLocaleString() should track Object.toString()', a.toString(), a.toLocaleString())


class C extends Object {
    public function C() {}
    public function toString():String { return 'custom string'}
}

var c:C = new C();
Assert.expectEq('Custom class toLocaleString should track toString', c.toString(), c.toLocaleString())

