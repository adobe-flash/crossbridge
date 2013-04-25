// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function switchTest() {
	var x = 10;

	switch(x) {
		case 10:
			x = 20;
			break;
		case 20:
			x = 30;
			break;
		case 30:
			x = 50;
			break;
		default:
			x = 100;
			break;
	}

	print(x);
}

switchTest();
