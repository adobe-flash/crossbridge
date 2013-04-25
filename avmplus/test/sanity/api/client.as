/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
// test client code

/*
   asc -config CONFIG::VERSION=0 -api 0 client.as
*/

package {
import avmplus.*;
CONFIG const V0 = CONFIG::VERSION==0;
CONFIG const V1 = CONFIG::VERSION==1;
CONFIG const V2 = CONFIG::VERSION==2;
CONFIG const V3 = CONFIG::VERSION==3;
CONFIG const V4 = CONFIG::VERSION==4;
CONFIG const V5 = CONFIG::VERSION==5;
CONFIG const V6 = CONFIG::VERSION==6;

CONFIG::V0 {
  use namespace AS3;
  public class B extends A {
    override public function m () { return 11 }
  }

  public class D extends A {
    override public function m () { return 11 }
    public function n () { return 21 }
    public function r () { return 51 }
    //public function s () { return 61 }
  }
  var o = new D();

  run(function() { return x0 }, false);
  run(function() { return x1 }, true);
  run(function() { return x2 }, true);
  run(function() { return x3 }, true);
  run(function() { return x4 }, true);
  run(function() { return x5 }, true);
  run(function() { return x12 }, true);
  run(function() { return x23 }, true);
  run(function() { return o.x==10 }, true);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, true);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, true);
  run(function() { return C.m()==50 }, false);
}

CONFIG::V1 {
  use namespace AS3;
  public class B extends A {
    override public function m () { return 11 }
  }

  public class D extends A {
    override public function m () { return 11 }
    override public function n () { return 21 }
    public function r () { return 51 }
    public function s () { return 61 }
  }

  var o = new D();

  run(function() { return x0 }, false);
  run(function() { return x1 }, false);
  run(function() { return x2 }, true);
  run(function() { return x3 }, true);
  run(function() { return x4 }, true);
  run(function() { return x5 }, true);
  run(function() { return x12 }, false);
  run(function() { return x23 }, true);
  run(function() { return o.x==10 }, false);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, true);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, false);
  run(function() { return C.m()==50 }, false);
}

CONFIG::V2 {
  public class B extends A {
    override public function m () { return 11 }
  }

  public class E extends A {
    override public function m () { return 11 }
    override public function n () { return 21 }
    //public function q () { return 41 }
    public function r () { return 51 }
    //public function s () { return 61 }
  }

  var o = new E();
  run(function() { return x0 }, false);
  run(function() { return x1 }, true);
  run(function() { return x2 }, false);
  run(function() { return x3 }, true);
  run(function() { return x4 }, true);
  run(function() { return x5 }, true);
  run(function() { return x12 }, false);
  run(function() { return x23 }, false);
  run(function() { return o.x==10 }, true);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, true);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, true);
  run(function() { return C.m()==50 }, false);
}

CONFIG::V3 {
  public class B extends A {
    override public function m () { return 11 }
  }

  public class D extends A {
    override public function m () { return 11 }
    override public function n () { return 21 }
    public function q () { return 41 }
    public function r () { return 51 }
    public function s () { return 61 }
  }

  var o = new D();
  run(function() { return x0 }, false);
  run(function() { return x1 }, false);
  run(function() { return x2 }, false);
  run(function() { return x3 }, false);
  run(function() { return x4 }, true);
  run(function() { return x5 }, true);
  run(function() { return x23 }, false);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, false);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, false);
  run(function() { return o.x==10 }, false);
}

CONFIG::V4 {
  public class B extends A {
    override public function m () { return 11 }
  }

  public class D extends A {
    override public function m () { return 11 }
    override public function n () { return 21 }
    public function q () { return 41 }
    public function r () { return 51 }
    public function s () { return 61 }
  }

  var o = new D();
  run(function() { return x0 }, false);
  run(function() { return x1 }, false);
  run(function() { return x2 }, false);
  run(function() { return x3 }, false);
  run(function() { return x4 }, false);
  run(function() { return x5 }, true);
  run(function() { return x23 }, false);
  run(function() { return o.x==10 }, false);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, false);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, false);
  run(function() { return C.m()==50 }, false);  // FIXME crashes jit
}

CONFIG::V5 {
  public class D extends A {
    override public function m () { return 11 }
    override public function n () { return 21 }
    public function q () { return 41 }
    public function r () { return 51 }
    public function s () { return 61 }
  }

  var o = new D();

  run(function() { return x0 }, false);
  run(function() { return x1 }, true);
  run(function() { return x2 }, false);
  run(function() { return x3 }, true);
  run(function() { return x4 }, true);
  run(function() { return x5 }, false);
  run(function() { return x23 }, false);
  run(function() { return o.x==10 }, false);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, false);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, false);
  run(function() { return C.m()==50 }, false);
}

CONFIG::V6 {
  public class D extends A {
    override public function m () { return 11 }
    override public function n () { return 21 }
    public function q () { return 41 }
    public function r () { return 51 }
    public function s () { return 61 }
  }

  var o = new D();

  run(function() { return x0 }, false);
  run(function() { return x1 }, false);
  run(function() { return x2 }, false);
  run(function() { return x3 }, false);
  run(function() { return x4 }, false);
  run(function() { return x5 }, false);
  run(function() { return x23 }, false);
  run(function() { return o.x==10 }, false);
  run(function() { return o.m()==11 }, false);
  run(function() { return o.n()==21 }, false);
  run(function() { return o.q()==41 }, false);
  run(function() { return o.r()==51 }, false);
  run(function() { return o.s()==61 }, false);
  run(function() { return C.m()==50 }, false);
}
}

