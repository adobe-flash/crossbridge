// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function testIncrementArray() {
	var x = [0, 0, 0, 0];
	var i = 2;
	var result = ++x[i];
	print(x[i]);
}

testIncrementArray();
