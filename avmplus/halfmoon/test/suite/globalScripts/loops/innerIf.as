// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

var x = 10;

for (var i = 0; i < 5; i++) {
	if (i % 2) {
		x += 20;
	}

	x *= 2;
}

print(x);
