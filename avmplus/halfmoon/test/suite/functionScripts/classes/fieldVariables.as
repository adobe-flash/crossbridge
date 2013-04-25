// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestPackage {
	public class TestClass {
		private var message;
		private var value;

		public function TestClass() {
			message = "hello world";
			value = 0;
		}

		public function printMessage() {
			this.value++;
			print(message + value);
		}
	}

	var x = new TestClass();
	x.printMessage();
}
