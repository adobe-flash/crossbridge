/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// 8.2.6 interfaces have traits but they only exist at compile-time
// therefore trying to access a super method will fail because the
// referenced trait does not exist on the implicit Object base class!
package TraitsNotAccessible {
    interface I {
        function f();
    }
    class X implements I {
        public function f() {
            return "x.I::f()";
        }
        public function g() {
            return super.f();
        }
    }

    public class TraitTest {
        var x : X = new X();
        public function doCall() : String {
            return x.I::f();
        }
        public function doSuperCall() : String {
            return x.g();
        }
    }
}

