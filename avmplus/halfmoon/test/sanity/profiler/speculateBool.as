// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
	function testbool(b) {
		if (b) { print("true"); }
		else { print("false"); }
	}

	for (var i = 0; i < 20; i++) {
		testbool(true);
	}
}
