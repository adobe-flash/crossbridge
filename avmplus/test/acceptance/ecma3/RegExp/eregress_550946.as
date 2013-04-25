/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
/*
*
* See http://bugzilla.mozilla.org/show_bug.cgi?id=550946
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_550946";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "550946";

var testcases = getTestCases();

function getTestCases() {
    var array:Array = new Array();
    var item:int = 0;
    var status:String = '';
    var actual:String = '';
    var expect:String= '';

 //    status = inSection(1);
    var str:String = "\u5F0A\u793E\u304C\u5236\u4F5C\u5354\u529B\u3057\u307Ehttp://link1.com/\u3000\u5C11\u5E74\u30BF\u30B1\u30B7\u3082\u30EA\u30CBhttp://link2.com/";
    var myPattern:RegExp = /(http:\/\/[\x21-\x7e]+)/gi;
    var match_array:Array = str.match(myPattern);
    expect = "http://link1.com/,http://link2.com/";
    actual = match_array.toString();
    array[item++] = Assert.expectEq( status, expect, actual);


    return array;
}

