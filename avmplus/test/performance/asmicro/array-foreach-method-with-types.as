/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Executing Array.forEach with a method with type annotations";

include "driver.as"

class C {
        function meth_types(e:uint,i:uint,a:Array){}
}

var a:Array = [];
for(var i:uint=0;i<1000;i++)
        a.push(i);

function doit():uint {
        var c:C = new C;
        for(var j:uint = 0; j < 100; j++)
                a.forEach(c.meth_types);
        return i*j;
}

TEST(doit, "array-foreach-method-with-types");

