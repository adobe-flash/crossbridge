/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Section 9.2 example with qualifiers
package Example_9_2_qual {

    interface I {
        function m();   // example erroneously has { } here
    }
    
    class A implements I {
        public function m() {
            return "a.I::m()";
        }
    }
        
    public class ExampleTest {
        var x : A = new A;
        public function doTest() : String {
            return x.I::m();
        }
    }

}
