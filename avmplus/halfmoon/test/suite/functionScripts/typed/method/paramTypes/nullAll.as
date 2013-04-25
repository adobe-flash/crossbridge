// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestNullString {
    function nullAll(testArray:Array, testObject:Object, testString:String, testInt:int, testNumber:Number) {
        print(testArray);
        print(testObject);
        print(testString);
        print(testInt);
        print(testNumber);
    }

    function passParam() {
        nullAll(null, null, null, null, null);
    }

    passParam();
}

