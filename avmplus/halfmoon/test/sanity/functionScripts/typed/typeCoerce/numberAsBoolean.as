// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testConditions(a:Number) {
    if (a) {
        print("Exists as number" + a);
    } else {
        print("Number not converted to boolean correctly" + a);
    }
}

testConditions(10);
testConditions(15);
testConditions(-5);
testConditions(0);
