/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package inlinetest {

	class FactorialWrapperTester {

		final function factorialWrapper(n):Number {
			return factorial(n);
			}

		//factorial is not trivially recursive.
		//it calls factorialWrapper, which calls factorial.
		//
		final function factorial(n):Number {

			if (n < 2) {
				return 1;
			}
			return n*factorialWrapper(n-1);
		}
	}
	var f:FactorialWrapperTester = new FactorialWrapperTester();
	trace(f.factorialWrapper(3)); //Outputs 6
} //package
