/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

include "driver.as"

var DESC = "arguments-object baseline: early-bindable class and method that doesn't use the arguments object";

class C {
    function push(v:*):void {
        x = v;
    }
    function get length(): uint { return 0 }
    function set length(n: uint): void { }
    var x:*;
}

function pushloop():uint {
    var a:C = new C;
    for ( var i:int=0 ; i < 100000 ; i+=10 ) {
        a.length = 0;
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
    }
    return a.length;
}

TEST(pushloop, "arguments-1");
