// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {

public class Constraint {
    public function Constraint():void {
    }

    public function satisfy():Constraint {
        return this;
    }
} // class Constraint

public class Variable {
    function Variable() {
    }
} // class Variable

public class Planner {
    public function Planner():void {
    }
    
    public function incrementalAdd(c:Constraint):void {
        var overridden:Constraint = new Constraint();
        while (overridden != null)
            overridden = overridden.satisfy();
    }
    
    public function incrementalRemove():void {
        var u:Constraint = new Constraint();
        if (10 == 20) {
            this.incrementalAdd(u);
        }
    }
} // class Planner

// Global variable holding the current planner.
var planner:Planner = null;

function deltaBlue():void {
    planner = new Planner();
    planner.incrementalRemove();
}


deltaBlue();

} // package
