// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
	function loop(x) {
		var sum = 0;
		sum += x;
		print(sum);
	}

	for (var i = 0; i < 30; i++) {
		if (i > 25) {
			loop(i + 0.5);
		} else {
			loop(i);
		}
	}
}
