// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestOptional {
    function testOptional(x:Number = 3.14159, y:int = 10, z:Number = 2.1723) {
        print(x);
        print(y);
        print(z);
    }
    
    testOptional();
    testOptional(2.718);
    testOptional(3132.232, 20);
    testOptional(123.12317, 50, 23877);

}
