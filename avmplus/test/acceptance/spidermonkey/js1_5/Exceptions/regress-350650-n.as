/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-350650-n.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 350650;
var summary = 'custom toString exception';
var actual = 'Error';
var expect = 'Error';

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------
function addtestcases() {

    //printBugNumber(BUGNUMBER);
  //printStatus(summary);

    function exc() {
        this.toString = function() {
            return "EXC";
        }
    }
    try {
        throw new exc();
    } catch (e) {
        Assert.expectEq(summary, "EXC", e.toString());
    }
}

