// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function innerForLoop() {
    var x = 0;

    for (var i = 1; i < 10; i++) {
        x *= i;
        for (var j = 1; j < 20; j++) {
            x += j;
            var y = 10;
        }
    }

    print(x);
    print(y);
}

innerForLoop();
