/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-167328.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 167328;
var summary = 'Normal error reporting code should fill Error object properties';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus(summary);

expect = 'TypeError: Error #1050'; // TypeError: Error #1050: Cannot convert Object to primitive.
try {
    var obj = {
        toString: function() {
            return new Object();
        }
    };
    var result = String(obj);
    actual = 'no error';
} catch(e) {
    actual = e.toString().substr(0, 22);
}
Assert.expectEq(summary, expect, actual);

