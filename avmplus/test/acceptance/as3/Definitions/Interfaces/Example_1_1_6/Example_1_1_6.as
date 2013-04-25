/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Section 1.1.6 example
// Also covers 1.1.6 no interface name required for public non-ambiguous method.
package Example_1_1_6 {

    interface Greetings {
        function hello();
        function goodmorning();
    }
    
    class Greeter implements Greetings {
        public function hello() {
            return "hello, world";
        }
        public function goodmorning() {
            return "goodmorning, world";
        }
    }
    
    public class ExampleTest {
        var greeter : Greetings = new Greeter();
        public function doHello() : String {
            return greeter.hello();
        }
        public function doGoodMorning() : String {
            return greeter.goodmorning();
        }
    }

}
