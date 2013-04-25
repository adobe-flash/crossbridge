// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package TestAdd {
    function doSum(x:int, y:int):int {
        if (x > y) {
            return x + y;
        } else {
            return x * y;
        }
    }

    function addNumbers() {
        print(doSum(10, 18));
    }

    addNumbers();
}
