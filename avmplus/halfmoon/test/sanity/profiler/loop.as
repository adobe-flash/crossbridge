// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


package {
	function loop(x) {
		var sum = 0;
		for (var i = 0; i < x; i++) {
			sum += i;
			print(i);
			sum += i;
		}

		return sum;
	}

	for (var i = 0; i < 20; i++) {
		loop(i);
	}
}
