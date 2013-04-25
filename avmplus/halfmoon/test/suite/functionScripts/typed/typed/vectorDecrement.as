// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testThing() {
    var i:int = 10;
    var testArray:Vector.<int> = new Vector.<int>(10);
    var qd:int = testArray[--i];
    print(qd);
}

testThing();
