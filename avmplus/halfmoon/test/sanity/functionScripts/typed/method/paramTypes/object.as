// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {

    function printHello(value:Object) {
        otherValue(value);
    }

    function otherValue(value:Object) {
        print(value);
        print(value.x);
    }

    var testObject:Object = new Object();
    testObject.x = "awesome";
    
    var otherObject:Object = new Object();
    otherObject.x = "sweet";


    printHello(testObject);
    printHello(otherObject);
}


