// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package FlogRayTracer {


public class RVector {
    public var x = 0.0;
    public var y = 0.0;
    public var z = 0.0;

    public function RVector(x, y, z) {
        this.x = (x ? x : 0);
        this.y = (y ? y : 0);
        this.z = (z ? z : 0);
    }

    public static function add(v, w) {
        return new RVector(w.x + v.x, w.y + v.y, w.z + v.z);
    }

    public function toString() {
        return 'RVector [' + this.x + ',' + this.y + ',' + this.z + ']';
    }
}

function renderScene(){
    var vectorOne = new RVector(0, 0, -5);
    var vectorTwo = new RVector(10, 12, 13);
    var testVector = RVector.add(vectorOne, vectorTwo);
    print(testVector);
}

renderScene();

} // package
