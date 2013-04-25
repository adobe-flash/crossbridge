// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function innerForArray() {
    var testArray = new Array(10);

    for (var i = 0; i < 10; i++) {
        testArray[i] = new Array(20);
        for (var j = 0; j < 20; j++) {
            testArray[i][j] = i * j;
        }
    }

    print(testArray[9][19]);
}

innerForArray();
