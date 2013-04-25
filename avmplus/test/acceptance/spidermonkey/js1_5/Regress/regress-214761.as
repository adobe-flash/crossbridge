/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-214761.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 214761;
var summary = 'Crash Regression from bug 208030';
var actual = 'No Crash';
var expect = 'No Crash';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

try
{
    var bar1 = new Array();
    bar1[0] = 'foo';
    //var bar2 = document.all && navigator.userAgent.indexOf('Opera') == -1;
    //var bar3 = document.getElementById && !document.all;
    //var bar4 = document.layers;
    function foo1() {
        return false;
    }
    function foo2() {
        return false;
    }
    function foo3() {
        return false;
    }
    function foo4() {
        return false;
    }
    function foo5() {
        return false;
    }
    function foo6() {
        return false;
    }
    function foo7() {
        return false;
    };
}
catch(e)
{
}

Assert.expectEq(summary, expect, actual);

