// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


function testSomething() {
    var j = 18;
    var testVariable = 26;
    var loopCondition = testVariable & (1 << 0);
    var jGreaterThan = (j >= 0);
    var evaluatedCondition = jGreaterThan && loopCondition;
    print("JGreaterThan: " + jGreaterThan + " Evaluated condition: " + evaluatedCondition);
}

testSomething();
