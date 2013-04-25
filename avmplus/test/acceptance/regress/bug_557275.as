/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
*
* See http://bugzilla.mozilla.org/show_bug.cgi?id=557275
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "557275";
// var VERSION = "";
// var TITLE   = "AvmCore::intern truncation";
// var bug = "557275";

import com.adobe.test.Assert;
var testcases = getTestCases();

function getTestCases() {
    var array:Array = new Array();
    var item:int = 0;
    var status:String = '';
    var actual:String = '';
    var expect:String= '';

    // this gyration is necessary to construct an integer atom
    // that will not fit into an int32
    var u = 0x0fffffff;
    for(var i = 0; i < 5; ++i)
        u += u;
    
    var o = {}
   
    o[u] = 'foo'

    status = 'test1';
    expect = 'foo';
    actual = o[8589934560];
    array[item++] = Assert.expectEq( status, expect, actual);

    delete o[u]

    status = 'test2';
    expect = undefined;
    actual = o[8589934560];
    array[item++] = Assert.expectEq( status, expect, actual);

    return array;
}
