/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// 9 get / set allowed in interfaces
package GetSet {
    interface A {
        function get a() : String;
        function set a(aa:String) : void;
        function get b() : String;
        function set c(cc:String) : void;
    }
    interface B {
        function set b(bb:String) : void;
        function get c() : String;
    }

    class X implements A {
        var a_ : String = "unset";
        var c_ : String = "unset";
        public function get a() : String {
            return "x.A::get a()";
        }
        public function set a(x:String) : void {
            a_ = "x.A::set a()";
        }
        public function getA() : String {
            return a_;
        }
        public function get b() : String {
            return "x.A::get b()";
        }
        public function set c(x:String) : void {
            c_ = "x.A::set c()";
        }
        public function getC() : String {
            return c_;
        }
    }
    class Y implements A, B {
        var a_ : String = "unset";
        var b_ : String = "unset";
        var c_ : String = "unset";
        public function get a() : String {
            return "y.A::get a()";
        }
        public function set a(x:String) : void {
            a_ = x;
        }
        public function getA() : String {
            return a_;
        }
        public function get b() : String {
            return "y.A::get b()";
        }
        public function set b(x:String) : void {
            b_ = x;
        }
        public function getB() : String {
            return b_;
        }
        public function get c() : String {
            return "y.B::get c()";
        }
        public function set c(x:String) : void {
            c_ = x;
        }
        public function getC() : String {
            return c_;
        }
    }

    public class GetSetTest {
        var x : X = new X();
        var y : Y = new Y();
        public function doGetAX() : String {
            return x.a;
        }
        public function doSetAX() : String {
            x.a = "ignored";
            return x.getA();
        }
        public function doGetBX() : String {
            return x.b;
        }
        public function doSetCX() : String {
            x.c = "ignored";
            return x.getC();
        }
        public function doGetAY() : String {
            return y.a;
        }
        public function doSetAY() : String {
            y.a = "y.A::set a()";
            return y.getA();
        }
        public function doGetBY() : String {
            return y.b;
        }
        public function doSetBY() : String {
            y.b = "y.B::set b()";
            return y.getB();
        }
        public function doGetCY() : String {
            return y.c;
        }
        public function doSetCY() : String {
            y.c = "y.A::set c()";
            return y.getC();
        }
    }
}

