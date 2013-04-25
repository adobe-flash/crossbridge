/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Verify that the implementation of one or more interfaces can
// be assembled via inheritance from one or more classes
package ImplementByExtension {
    interface I {
        function f();
    }
    interface J {
        function g();
    }
    interface I1 extends I { }
    interface I2 extends I {
        function g();
    }
    interface J1 extends J { }
    interface J2 extends J {
        function f();
    }
    interface K extends I, J { }
    interface K1 extends I1, J1 { }
    
    class A {
        public function f() {
            return "A::f()";
        }
    }
    class AX extends A { }
    class B {
        public function g() {
            return "B::g()";
        }
    }
    class BX extends B { }
    class C extends A {
        public function g() {
            return "C::g()";
        }
    }
    class CX extends C { }
    class CY extends AX {
        public function g() {
            return "CY::g()";
        }
    }
    class A1 extends A implements I { }
    class A2 extends A implements I1 { }
    class A3 extends AX implements I { }
    class A4 extends AX implements I1 { }
    class A5 extends A implements K {
        public function g() {
            return "A5::g()";
        }
    }
    class A6 extends A implements I2 {
        public function g() {
            return "A6::g()";
        }
    }
    class B1 extends B implements K1 {
        public function f() {
            return "B1::f()";
        }
    }
    class B2 extends BX implements K {
        public function f() {
            return "B2::f()";
        }
    }
    class B3 extends BX implements J2 {
        public function f() {
            return "B3::f()";
        }
    }
    class C1 extends C implements I, J { }
    class C2 extends CX implements I, J { }
    class C3 extends C implements I, J1 { }
    class C4 extends CX implements I1, J { }
    class C5 extends CX implements I1, J1 { }
    class C6 extends CX implements I2 { }
    class C7 extends CX implements K1 { }
    class C8 extends CY implements I1, J1 { }
    class C9 extends CY implements K1 { }

    public class ImplementTest {
        var a1 : A1 = new A1;
        var a2 : A2 = new A2;
        var a3 : A3 = new A3;
        var a4 : A4 = new A4;
        var a5 : A5 = new A5;
        var a6 : A6 = new A6;
        var b1 : B1 = new B1;
        var b2 : B2 = new B2;
        var b3 : B3 = new B3;
        var c1 : C1 = new C1;
        var c2 : C2 = new C2;
        var c3 : C3 = new C3;
        var c4 : C4 = new C4;
        var c5 : C5 = new C5;
        var c6 : C6 = new C6;
        var c7 : C7 = new C7;
        var c8 : C8 = new C8;
        var c9 : C9 = new C9;
        public function doCallAF() : String {
            return a1.f() + "," + a2.f() + "," + a3.f() + "," + a4.f() + "," + a5.f() + "," + a6.f();
        }
        public function doCallAG() : String {
            return a5.g() + "," + a6.g();
        }
        public function doCallBF() : String {
            return b1.f() + "," + b2.f() + "," + b3.f();
        }
        public function doCallBG() : String {
            return b1.g() + "," + b2.g() + "," + b3.g();
        }
        public function doCallCF() : String {
            return c1.f() + "," + c2.f() + "," + c3.f() + "," + c4.f() + "," + c5.f() + "," + c6.f() + "," + c7.f() + "," + c8.f() + "," + c9.f();
        }
        public function doCallCG() : String {
            return c1.g() + "," + c2.g() + "," + c3.g() + "," + c4.g() + "," + c5.g() + "," + c6.g() + "," + c7.g() + "," + c8.g() + "," + c9.g();
        }
    }
}

