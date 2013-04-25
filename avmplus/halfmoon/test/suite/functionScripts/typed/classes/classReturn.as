// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
    class TestClass {
	public function TestClass() {

	}

	public function sayHello():void {
		print("hello world");
	}

    public function calculateSomething(value:Number) {
        return value * 10;
    }
}

function newInstance() {
    var instanceHere:TestClass = new TestClass();
    var result = instanceHere.calculateSomething(50);
    print(result);
}

newInstance();
}
