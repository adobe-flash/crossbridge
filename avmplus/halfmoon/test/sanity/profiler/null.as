// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


// Test null between halfmoon and profiler
// halfmoon represents null as kObjectNull = 1
// codegenlir represents null as pure 0
// so we need a case where halfmoon is jitted, and calls codegenlir code
package {
	class Packet { }

	function codegenlir(x:Packet) {
		if (x == null) {
			print("NULL CODEGENLIR");
		} else {
			print("I'm not null");
		}
	}

	function halfmoonCode(counter:int) {
		var x:Packet = new Packet();
		// Assumes profiler limit is less than 15 method calls
		if (counter > 15) {
			x = null;
			codegenlir(x);
		}
	}

	for (var i:int = 0; i < 20; i++) {
		halfmoonCode(i);
	}

}
