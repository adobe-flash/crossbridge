// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {


public class Constraint {
    public function Constraint() {
    }
    
    public function addConstraint() {
      this.addToGraph();
    }
    
    
    public function addToGraph() {}
} 

public class BinaryConstraint extends Constraint {
    public function BinaryConstraint() {
      this.addConstraint();
    }
    
    override public function addToGraph() {
    }
} 


var firstOne = new BinaryConstraint();
var secondOne = new BinaryConstraint();
} // package
