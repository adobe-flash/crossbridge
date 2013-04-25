// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestInheritance {
	public class BaseClass {
		private var testVariable:String;

		public function BaseClass() {
			this.testVariable = "base class";
		}

		public function setMessage(newMessage:String) {
			this.testVariable = newMessage;
		}

		public function printMessage() {
			print(this.testVariable);
		}
	}

	public class DerivedClass extends BaseClass {
		private var derivedVariable:String;
		public function DerivedClass() {
            super();
		}

		public override function printMessage() {
			print("Derived class");
		}
	}

	public class TripleDerivedClass extends DerivedClass {
		public function TripleDerivedClass() {
			super();
		}
	}

	var x:TripleDerivedClass = new TripleDerivedClass();
	x.printMessage();
}
