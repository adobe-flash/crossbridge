// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testBreakLoop() {
    var i = 10;

    while (true) {
        i++;
        if (i > 20) {
            return i;
        }
    }

    return 0;
}

print(testBreakLoop());
