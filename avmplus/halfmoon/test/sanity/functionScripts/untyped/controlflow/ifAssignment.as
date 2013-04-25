// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function fannkuch(n) {
	var perm = 10;
	var flipsCount = 0;
	var k;

	if (!((k = perm) == 0)) {
		flipsCount++;
	}

	print(flipsCount);
}

var res=fannkuch(8);
