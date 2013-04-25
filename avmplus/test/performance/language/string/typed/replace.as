 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

include '../timetest.as'
var size:int = 700;
var str:String;
for (var i:int=0; i < size; i++)
    str += "(123) 456-7890 asdf asdf asdf (123) 456-7890 asdf asdf asdf asdf asdf asdf (123) 456-7890 blah blah blah () blah";
var regex:RegExp = new RegExp(/^\(\d{3}\) \d{3}-\d{4}$/);

function replace(loops:int):void {
    for (var i:int=0; i<loops; i++)
        str.replace(regex,"(415) 832-5555");
}
timetest(replace, 1000, 100);
