// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {

public class Constraint {
    public var strength:String;
    
    public function Constraint(strength:String) {
      this.strength = strength;
    }
    
    public function addConstraint() {
      this.addToGraph();
    }
    
    // functions to be overridden by children
    public function addToGraph() {}
} // class Constraint

public class BinaryConstraint extends Constraint {
    
    protected var v2:String;
    protected var direction:int;
    
    public function BinaryConstraint(var2:String, strength:String) {
      super(strength);
      this.addConstraint();
    }
    
    override public function addToGraph() {
      print("Binary add to graph");
    }
} // class BinaryConstraint


function chainTest(n:int) {
  var equalityConstraint:BinaryConstraint = new BinaryConstraint("var2", "some strength");
}

function deltaBlue() {
  chainTest(100);
}

deltaBlue();
} // package
