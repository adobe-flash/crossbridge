/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Executing Array.forEach with an empty function";

include "driver.as"

function plainfunc(e,i,a){}
var a:Array = [];
for(var i:uint=0;i<1000;i++)
        a.push(i);

function doit():uint {
        for(var j:uint = 0; j < 100; j++)
                a.forEach(plainfunc);
        return i*j;
}

TEST(doit, "array-foreach-func");

