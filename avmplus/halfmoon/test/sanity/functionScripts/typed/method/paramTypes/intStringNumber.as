// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {

    function printHello(value:int, stringValue:String, numValue:Number) {
        otherValue(value, stringValue, numValue);
    }

    function otherValue(value:int, stringValue:String, numValue:Number) {
        print(stringValue + value + numValue);
    }

    printHello(5, "hello", 3.14159);
    printHello(10, "sweet", 2.81237);
}


