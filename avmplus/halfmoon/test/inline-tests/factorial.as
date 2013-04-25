/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package inlinetest {

	class FactorialTester {

		final function factorial(n):Number {

			if (n < 2) {
				return 1;
			}
			return n*factorial(n-1);
		}
	}

	f = new FactorialTester();

	trace(f.factorial(3)); //Outputs 6
} //package
