// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package TestPackage {
	public class TestClass {
		public var message:String;
		public var intValue:int;
        public var uintValue:uint;
        public var objectValue:Object;
        public var arrayValue:Array;
        public var numberValue:Number;
        public var booleanValue:Boolean;

		public function TestClass() {
            message = "hello world";
			intValue = 0;
            uintValue = -1;
            objectValue = new Object();
            booleanValue = false;
            arrayValue = new Array(5);
            arrayValue[3] = "set";
            numberValue = 3.14159;
		}

        public function printValues():void {
            print(this.message);
            print(this.objectValue);
            print(this.intValue);
            print(this.uintValue);
            print(this.numberValue);
            print(this.arrayValue);
            print(this.booleanValue);
        }

        public function changeValues():void {
            this.intValue++;
            this.message = " changed";
            this.numberValue = 2.71349;
            this.objectValue = new Object();
            this.uintValue = 5;
            arrayValue[2] = "also set";
            booleanValue = true;
        }
	}

    var x:TestClass = new TestClass();
    x.printValues();
    x.changeValues();
    x.printValues();
}
