// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {

    function printHello(value:int, numValue:Number, stringValue:String) {
        otherValue(value, numValue, stringValue);
    }

    function otherValue(value:int, numValue:Number, stringValue:String) {
        print(stringValue + value + numValue);
    }

    printHello(5, 3.14159, "hello");
    printHello(10, 2.81237, "awesome");
}


