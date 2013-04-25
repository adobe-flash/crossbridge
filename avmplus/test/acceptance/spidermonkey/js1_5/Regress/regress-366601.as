/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-366601.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 366601;
var summary = 'Switch with more than 64k atoms';
var actual = '';
var expect = '';

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------
function addtestcases() {

    //printBugNumber(BUGNUMBER);
  //printStatus(summary);

    var N = 100 * 1000;
    var array = Array(N);

    var f = function(a) {
        x = []
        for (var i = 0; i != N; ++i)
            x[i] = i;
        switch (a) {
            case "a":
            case "b":
            case "c":
                return null;
        }
        return x;
    }
    var r = f("a");
    if (r !== null) throw "Unexpected result: bad switch label";

    Assert.expectEq(summary, expect, actual);

}

