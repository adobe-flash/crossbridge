// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


function testSomething() {
    var this_array:Vector.<int> = new Vector.<int>(10);
    var i:int = 0;
    var someConstant:int = 20;
    this_array[i++] = someConstant + 30;
    print(this_array); 
}

testSomething();
