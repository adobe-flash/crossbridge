// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {


public class BinaryConstraint {
    protected var v1;
    protected var v2;
    protected var direction;
    
    public function BinaryConstraint(var1, var2, strength) {
      this.v1 = var1;
      this.v2 = var2;
      this.chooseMethod("var");
    }

    public function returnTrue() {
        return true;
    }
    
    public function chooseMethod(mark) {
        this.direction = (returnTrue()) ? "NORTH" : "south";
    }

    public function printDirection() {
        print(this.direction);
    }
} 

function deltaBlue() {
    var x = new BinaryConstraint("hello", "world", 3);
    x.printDirection();
}

deltaBlue();
} // package
