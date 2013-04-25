// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {

    function printHello(value:Boolean, message:String, testObject:Object) {
        otherValue(value, message, testObject);
    }

    function otherValue(value:Boolean, message:String, testObject:Object) {
        print(value);
        print(message);
        print(testObject);
    }

    printHello(true, "hello", new Object());
    printHello(false, "sweet", new Object());
}


