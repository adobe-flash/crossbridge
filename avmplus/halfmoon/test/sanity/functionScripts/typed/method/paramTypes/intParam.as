// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {

    function printHello(value:int) {
        otherValue(value);
    }

    function otherValue(value:int) {
        print(value);
    }

    printHello(5);
    printHello(109);
}


