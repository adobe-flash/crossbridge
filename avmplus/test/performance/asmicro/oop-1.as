/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Object-oriented programming using prototype methods";
include "driver.as"

function makePrototype(proto, methods) {
    function f() {}
    f.prototype = proto;
    var o = new f;
    for ( var x in methods )
        o[x] = methods[x];
    return o;
}

function Obj(x, y, z) {
    this._x = x;
    this._y = y;
    this._z = z;
}
Obj.prototype =
    makePrototype(Object.prototype,
                  { x: function () { return this._x },
                    y: function () { return this._y },
                    z: function () { return this._z },
                    move: function (xrel, yrel, zrel) { this._x += xrel; this._y += yrel; this._z += zrel },
                    volume: function () { throw new Error("Obj.volume is abstract") },
                    toString: function() { return "(OBJ " + this.x() + " " + this.y() + " " + this.z() + ")" }
                  });

function Sphere(x, y, z, r) {  // (x,y,z) is the center
    this._r = r;
    Obj.call(this, x, y, z);
}
Sphere.prototype =
    makePrototype(Obj.prototype,
                  { volume: function () { return 4/3 * Math.PI * this.r() * this.r() * this.r() },
                    r: function () { return this._r },
                    toString: function() { return "(SPHERE " + this.x() + " " + this.y() + " " + this.z() + " " + this.r() + ")" }
                  });

function TranslucentSphereOrig(x, y, z, r, alpha) {
    this._alpha = alpha;
    Sphere.call(this, x, y, z, r);
}
var TranslucentSphere = TranslucentSphereOrig;
TranslucentSphere.prototype =
    makePrototype(Sphere.prototype,
                  { alpha: function () { return this._alpha } });

function BoxOrig(x, y, z, l) {  // (x,y,z) is some corner
    this._l = l;
    Obj.call(this, x, y, z);
}
var Box = BoxOrig;
Box.prototype =
    makePrototype(Obj.prototype,
                  { volume: function () { return this.l() * this.l() * this.l() },
                    l: function () { return this._l },
                    toString: function() { return "(BOX " + this.x() + " " + this.y() + " " + this.z() + " " + this.l() + ")" }
                  });

function CompositeOrig(o1, o2) {
    this._o1 = o1;
    this._o2 = o2;
    Obj.call(this, 0, 0, 0);
}
var Composite = CompositeOrig;
Composite.prototype =
    makePrototype(Obj.prototype,
                  { volume: function () { return this.o1().volume() + this.o2().volume() },
                    move: function (x, y, z) { this.o1().move(x, y, z); this.o2().move(x, y, z) },
                    o1: function () { return this._o1 },
                    o2: function () { return this._o2 },
                    toString: function () { return "(COMPOSITE " + this.o1() + " " + this.o2() + ")" }
                  });

function ooploop() {
    var g =
        new Composite(new TranslucentSphere(0,0,0,7,0.25),
                      new Box(1,1,1,4));
    var v = 0;
    for ( var i:uint=0 ; i < 100000 ; i++ ) {
        g.move(1,2,3);
        v += g.volume();
    }
}

TEST(ooploop, "oop-1");
