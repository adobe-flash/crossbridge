// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testDoubleVar() {
    var sum:int = 10;

    for (var i:int = 10; i < 100; i++) {
        sum += i;
    }

    for (var i:int = 100; i < 1000; i++) {
        sum += i;
    }

    print(sum);
}

testDoubleVar();
