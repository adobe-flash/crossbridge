// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


function testSomething() {
    var j:int = 18;
    var testVariable:int = 26;
    var loopCondition:int = testVariable & (1 << 0);
    var jGreaterThan:Boolean = (j >= 0);
    var evaluatedCondition:Boolean = jGreaterThan && loopCondition;
    print("JGreaterThan: " + jGreaterThan + " Evaluated condition: " + evaluatedCondition);
}

testSomething();
