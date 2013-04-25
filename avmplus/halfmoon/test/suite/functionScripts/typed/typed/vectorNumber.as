// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testVector() {
    var result:Vector.<Number> = new Vector.<Number>(10);
    result[3] = 3.14159;
    print(result[3]);
    print(result);
}

testVector();
