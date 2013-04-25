// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testBranchOnUndefined(message:String=undefined) {
    if (message) {
        print(message);
    } else {
        print(message);
    }
}

testBranchOnUndefined();
testBranchOnUndefined("should be");
