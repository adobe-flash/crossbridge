// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testSomething() {
		var testArray = new Array(10);
		var index = 0;
		var x = 50;

		testArray[index] = 0;
		testArray[index++] ^= x;

		print(testArray[index - 1]);
}

testSomething();
