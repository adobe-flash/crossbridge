/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Section 9.2 compile-time constant example:
// this injects a namespace declaration to test the example's
// assertion that an interface name is a compile-time constant
// that is equivalent to a namespace...
package Example_9_2_const {

    interface I { function f(); }
    
    
    class A implements I {
        public function f() {
            return "A::f";
        }
    }
    var a : A = new A();
    public class ExampleTest {
        public function doTest() : String {
            return a.I::f();
        }
    }
    
}
