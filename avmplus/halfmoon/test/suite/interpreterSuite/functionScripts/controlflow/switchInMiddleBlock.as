// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function BigInteger() {
	var am = 0;
	var a = null;
	var defaultAm = 3;

	if(a != null) {
		print("not null");
	}

	print("Default av: " + defaultAm);

	// set the am function
	switch (defaultAm) {
		case 1:
		am = 1;
		break;
	case 2:
		am = 2;
		break;
	case 3:
		am = 3;
		break
	case 4:
		am = 4;
		break;
	default:
		print('Error: no am value is set');
	}

	print(am);
}   

BigInteger();
