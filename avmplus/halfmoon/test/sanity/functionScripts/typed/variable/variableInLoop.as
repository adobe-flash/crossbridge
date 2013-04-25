// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function definedLater() {
    var x:int = 10;
    var i:int = 0;

    for (i = 0; i < 10; i++) {
        for (var j:int = 0; j < 10; j++) {
            x++;
        }
    }

    print(x);
    print(j);
}

definedLater();
