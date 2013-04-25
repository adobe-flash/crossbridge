/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// 9 interface can extend multiple interfaces
package ExtendMultipleInterfaces {
    interface A {
        function a();
    }
    interface B {
        function b();
    }
    interface C {
        function c();
    }
    interface D {
        function d();
    }
    
    interface I1 extends A {
    }
    interface I2 extends A, B {
    }
    interface I3 extends C {
        function d();
    }
    interface I4 extends A, B, I3 {
    }

    class X1 implements I1 {
        public function a() {
            return "x1.A::a()";
        }
    }
    class X2 implements I2 {
        public function a() {
            return "x2.A::a()";
        }
        public function b() {
            return "x2.B::b()";
        }
    }
    class X3 implements I3 {
        public function c() {
            return "x3.C::c()";
        }
        public function d() {
            return "x3.I3::d()";
        }
    }
    class X4 implements I4 {
        public function a() {
            return "x4.A::a()";
        }
        public function b() {
            return "x4.B::b()";
        }
        public function c() {
            return "x4.C::c()";
        }
        public function d() {
            return "x4.I3::d()";
        }
    }
    
    interface J1 extends I1, B {
    }
    interface J2 extends I2, C {
    }
    interface J3 extends I2, I3 {
    }
    interface J4 extends J1, I3 {
    }
    
    class Y1 implements J1 {
        public function a() {
            return "y1.A::a()";
        }
        public function b() {
            return "y1.B::b()";
        }
    }
    class Y2 implements J2 {
        public function a() {
            return "y2.A::a()";
        }
        public function b() {
            return "y2.B::b()";
        }
        public function c() {
            return "y2.C::c()";
        }
    }
    class Y3 implements J3 {
        public function a() {
            return "y3.A::a()";
        }
        public function b() {
            return "y3.B::b()";
        }
        public function c() {
            return "y3.C::c()";
        }
        public function d() {
            return "y3.I3::d()";
        }
    }
    class Y4 implements J4 {
        public function a() {
            return "y4.A::a()";
        }
        public function b() {
            return "y4.B::b()";
        }
        public function c() {
            return "y4.C::c()";
        }
        public function d() {
            return "y4.I3::d()";
        }
    }

    public class ExtendTest {
        var x1 : X1 = new X1();
        var x2 : X2 = new X2();
        var x3 : X3 = new X3();
        var x4 : X4 = new X4();
        var y1 : Y1 = new Y1();
        var y2 : Y2 = new Y2();
        var y3 : Y3 = new Y3();
        var y4 : Y4 = new Y4();
        public function doTestX1() : String {
            return x1.a();
        }
        public function doTestX2() : String {
            return x2.a() + "," + x2.b();
        }
        public function doTestX3() : String {
            return x3.c() + "," + x3.d();
        }
        public function doTestX4() : String {
            return x4.a() + "," + x4.b() + "," + x4.c() + "," + x4.d();
        }
        public function doTestY1() : String {
            return y1.a() + "," + y1.b();
        }
        public function doTestY2() : String {
            return y2.a() + "," + y2.b() + "," + y2.c();
        }
        public function doTestY3() : String {
            return y3.a() + "," + y3.b() + "," + y3.c() + "," + y3.d();
        }
        public function doTestY4() : String {
            return y4.a() + "," + y4.b() + "," + y4.c() + "," + y4.d();
        }
    }
}

