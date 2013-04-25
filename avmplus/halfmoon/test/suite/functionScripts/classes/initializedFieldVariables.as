// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestPackage {
	public class TestClass {
		private var message;
        private var queue;
		const TEST_VALUE = 20;
		public var TOTAL_VALUE = TEST_VALUE & 0xFF;

		public function TestClass(testQueue) {
            print("Test queue is: " + testQueue);
			message = "hello world";
		}

		public function printMessage() {
			print(message);
			print(TOTAL_VALUE);
		}
	}

    var queueParam = null;
	var x = new TestClass(queueParam);
	x.printMessage();
}
