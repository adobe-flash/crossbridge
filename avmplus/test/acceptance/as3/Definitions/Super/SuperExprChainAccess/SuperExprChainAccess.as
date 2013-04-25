/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// ActionScript file
package SuperExprChainAccess {

    class base {
        function f() : String {
            return "base f()";
        }
        function g() : String {
            return "base g()";
        }
        function i() : String {
            return "base i()";
        }
    }
    
    class middle1 extends base {
        override function f() : String {
            return "middle1 f()";
        }
        function h() : String {
            return "middle1 h()";
        }
    }
    
    class middle2 extends middle1 {
        override function h() : String {
            return "middle2 h()";
        }
        override function i() : String {
            return "middle2 i()";
        }
        function callh() : String {
            return super.h();
        }
        function calli() : String {
            return super.i();
        }
    }
    
    class derived extends middle2 {
        override function f() : String {
            return "derived f()";
        }
        override function g() : String {
            return "derived g()";
        }
        
        public function callf1() : String {
            return f();
        }
        public function callf2() : String {
            return super.f();
        }
        public function callg1() : String {
            return g();
        }
        public function callg2() : String {
            return super.g();
        }
        public function callh1() : String {
            return h();
        }
        public function callh2() : String {
            return super.h();
        }
        public function callh3() : String {
            return callh();
        }
        public function calli1() : String {
            return i();
        }
        public function calli2() : String {
            return super.i();
        }
        public function calli3() : String {
            return calli();
        }
    }
    
    public class SuperExprChainAccess extends derived { }
}
