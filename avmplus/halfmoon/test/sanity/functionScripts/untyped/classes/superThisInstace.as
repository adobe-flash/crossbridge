// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {

public class Constraint {
    protected var value;
    public function Constraint() {
        this.value = 10;
    }
    
    public function addToGraph() {
        print(this.value);
    }
} 

public class BinaryConstraint extends Constraint {
    public function BinaryConstraint() {
      super();
      this.addToGraph();
    }
   
    override public function addToGraph() {
      super.addToGraph();
    }
    
} // class BinaryConstraint


new BinaryConstraint();
} // package
