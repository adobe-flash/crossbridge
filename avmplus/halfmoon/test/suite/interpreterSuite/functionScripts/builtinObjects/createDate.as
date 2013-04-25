// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function createDate() {
    var start = new Date(8000);

    // Kill some time
    var y = 0;
    for (var i = 0; i < 10000; i++) {
       y += i; 
    }

    var total = new Date(120000) - start;
    print(total);
}

createDate();
