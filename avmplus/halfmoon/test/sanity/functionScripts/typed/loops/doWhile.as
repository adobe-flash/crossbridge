// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function doWhileLoop() {
	var x:int = 0;
	var sum:int = 0;

	do {
		sum += x;
		x--;
	} while (x > 0);

	print(x);
	print("sum: " + sum + " x: " + x);
}

doWhileLoop();
