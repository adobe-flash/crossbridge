// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {

public class OrderedCollection {
    private var elms:Array;
    
    public function OrderedCollection() {
        this.elms = new Array();
    }
    
    public function at(index:int):* {
        return this.elms[index];
    }
    
    public function size():int {
        return this.elms.length;
    }
} // class OrderedCollection


public class Constraint {
    public function Constraint():void {
    }
} // class Constraint

public class Variable {
    function Variable() {
    }
} // class Variable

public class Planner {
    public var currentMark:int;
    
    public function Planner():void {
        this.currentMark = 0;
    }
    
    public function incrementalAdd(c:Constraint):void {
        var mark:int = this.newMark();
        var overridden:Constraint = new Constraint(null);
        while (overridden != null)
            overridden = overridden.satisfy(mark);
    }
    
    public function incrementalRemove():void {
        var out:Variable = new Variable();
        var unsatisfied:OrderedCollection = new OrderedCollection();
        for (var i:int = 0; i < unsatisfied.size(); i++) {
            var u:Constraint = unsatisfied.at(i);
            if (u.strength == strength) {
                this.incrementalAdd(u);
            }
        }
    }
    
    public function newMark():int {
        return ++this.currentMark;
    }
} // class Planner

function projectionTest(n:int):void {
    planner = new Planner();
    planner.incrementalRemove();
}

// Global variable holding the current planner.
var planner:Planner = null;

function deltaBlue():void {
    projectionTest(100);
}


deltaBlue();

} // package
