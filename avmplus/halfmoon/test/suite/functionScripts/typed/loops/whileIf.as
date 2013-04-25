// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function whileIfLoop() {
    var m:int = 1;
    var c:int = 0;

    while (m < 5) {
        if(m > 2) { 
            c++;
        }

        m++;
    }

    print(c);
}

whileIfLoop();
