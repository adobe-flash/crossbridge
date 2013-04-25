// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
    function testString() {
        var testString:String = "hello " + "world";
        var otherString:String = "hello world";
        var isEqual:Boolean = testString == otherString;
        print("Is equal: " + isEqual);
    }
    
    testString();

}
