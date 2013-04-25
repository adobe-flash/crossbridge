/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// 9.4 example (excluding error cases)
package Example_9_4 {

    interface T {
        function f();
    }
    
    interface U {
        function f();
        function g();
    }
    
    interface V extends T, U {
        function h();
    }
    
    class A implements V {
        public function f() {
            return "a.{T,U}::f()";
        }
        public function g() {
            return "a.U::g()";
        }
        public function h() {
            return "a.V::h()";
        }
    }

    public class ExampleTest {
        var a : A = new A;
        var t : T = a;
        var u : U = a;
        var v : V = a;
        var o = a;
        public function doUniqueTest() : String {
            return t.f() + "," + u.g();
        }
        public function doBlendTest() : String {
            return v.g() + "," + v.h();
        }
        public function doImplementsTest() : String {
            return a.g() + "," + a.U::f();
        }
        public function doUntypedTest() : String {
            return o.T::f();
        }
    }
}

