// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function nestedIf() {
    var x:Number = 10;
    var y:int = 30;
    var z:Number = 0;

    if (x < 20) {
        if (y > 30) {
            z = 50;
        } else {
            z = 20;
        }
    } else {
        z = 10;
    }

    print(z);
}

nestedIf();
