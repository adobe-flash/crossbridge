// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function whileBreak() {
    var x:int = 10;

    while (true) {
        x++;

        if (x > 50) {
            x = 100;
            break;
        }
    }

    print(x);
}

whileBreak();
