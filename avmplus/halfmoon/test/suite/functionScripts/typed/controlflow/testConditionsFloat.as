// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testConditions() {
    var x:Number = 10;
    print("Equals: " + (x == 15)); 
    print("Not Equals: " + (x != 15));
    print("Less than: " + (x < 15));
    print("Greater than: " + (x > 15));
    print("Strict equals: " + (x === 15));
    print("Strict not equals: " + (x !== 100));
    print("Greater equal: " + (x >= 15));
    print("Less equal: " + (x <= 15));
}

testConditions();
