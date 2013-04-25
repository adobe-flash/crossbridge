/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-346027.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 346027;
var summary = 'Date.prototype.setFullYear()';
var actual = '';

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------
function addtestcases() {

    //printBugNumber(BUGNUMBER);
  //printStatus(summary);

    for (var i = 1600; i < 2400; i=i+3) {
        var d = new Date();
        d.setFullYear(i);
        actual = d.getFullYear();

        Assert.expectEq(summary, i, actual);

    }
}

