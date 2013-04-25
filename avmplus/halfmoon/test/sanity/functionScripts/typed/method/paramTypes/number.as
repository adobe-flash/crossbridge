// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {

    function printHello(value:Number) {
        otherValue(value);
    }

    function otherValue(value:Number) {
        print(value);
    }

    printHello(3.14159);
    printHello(2.71828);
}


