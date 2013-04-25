// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestVector {

    function testVectorInt() {
        var testVector:Vector.<int> = new Vector.<int>(10);
        testVector[0] = 10;
        testVector[5] = 30;
        print(testVector);
    }
    
    testVectorInt();
}
