/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
*
* See http://bugzilla.mozilla.org/show_bug.cgi?id=557933
*
*/
//-----------------------------------------------------------------------------

import com.adobe.test.Assert;
// var SECTION = "557933";
// var VERSION = "";
// var TITLE   = "parseIndex/getIntItem reject leading zeroes";
// var bug = "557933";

var testcases = getTestCases();

function concat_strs(a,b)
{
    return a + b;
}

function getTestCases() {
    var array:Array = new Array();
    var item:int = 0;
    
    var tests = [
        { key: 9,                       expect: 1 },
        { key: "9",                     expect: 1 },
        { key: "09",                    expect: 31 },
        { key: "009",                   expect: 3 },
        { key: "0009",                  expect: 4 },
        { key: 8,                       expect: 8 },
        { key: 0x8,                     expect: 8 },
        { key: 0x08,                    expect: 8 },
        { key: "8",                     expect: 8 },
        { key: "0x8",                   expect: 9 },
        { key: "0x08",                  expect: 10 },
        { key: -7,                      expect: 14 },
        { key: -07,                     expect: 14 },
        { key: -007,                    expect: 14 },
        { key: "-7",                    expect: 14 },
        { key: "-07",                   expect: 15 },
        { key: "-007",                  expect: 16 },
        { key: 6,                       expect: 24 },
        { key: 6.0,                     expect: 24 },
        { key: 6.00,                    expect: 24 },
        { key: 06.0,                    expect: 24 },
        { key: 06.00,                   expect: 24 },
        { key: 006.0,                   expect: 24 },
        { key: 006.00,                  expect: 24 },
        { key: "6",                     expect: 24 },
        { key: "6.0",                   expect: 25 },
        { key: "6.00",                  expect: 26 },
        { key: "06.0",                  expect: 27 },
        { key: "06.00",                 expect: 28 },
        { key: "006.0",                 expect: 29 },
        { key: "006.00",                expect: 30 },
        { key: concat_strs("0","9"),    expect: 31 },
    ]

    var o:* = []; // must be type *, not Array

    for (var i = 0; i < tests.length; ++i)
    {
        o[tests[i].key] = i;
    }
        
    for (var i = 0; i < tests.length; ++i)
    {
        array[item++] = Assert.expectEq( String(item) + ": " + typeof(tests[i].key) + " " + String(tests[i].key), tests[i].expect, o[tests[i].key]);
    }

    return array;
}
