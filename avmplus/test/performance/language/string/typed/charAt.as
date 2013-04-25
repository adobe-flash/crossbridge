/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

include '../timetest.as'

if (CONFIG::desktop) {
    var size:int = 50000;
} else {
    var size:int = 1000;
}
var myString:String = "";
for(var j:int = 0; j < size; j++)
    myString += "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 abcdefghijklmnopqrstuvwxyz1234567890";

var myStringLen:int = myString.length;

function stringCharAt(loops:int):void {
    for (var i:int =0; i < myStringLen; i++)
       myString.charAt(i);
}

timetest(stringCharAt);
