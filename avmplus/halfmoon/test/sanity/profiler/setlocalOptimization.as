// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {

	function returnVal() {
		return 10;
	}

	function safepoint(x) {
		var y = x + 10;
		y += returnVal();
		var z = y - 10;
		print(z);
	}

	for (var i = 0; i < 20; i++) {
		safepoint(i);

	}
}
