// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testContinue() {
    var x:int = 0;

    for (var i:int = 0; i < 10; i++) {
        if (i > 5) {
            continue;
        }
        
        x += i;        
    }

    print(x);
}

testContinue();
