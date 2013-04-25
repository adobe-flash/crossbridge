// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package deltablue {
public class OrderedCollection {
    private var elms:Array;
    
    public function OrderedCollection() {
        this.elms = new Array();
    }
    
    public function add(elm):void {
        this.elms[elms.length] = elm;
    }
} // class OrderedCollection


public class Constraint {
    public function Constraint():void {
    }
    
    public function destroyConstraint():void {
		print("Destroying");
		planner.incrementalRemove(this);
    }
} // class Constraint

public class Variable {
    public var value:int;
    
    function Variable(initialValue:int = 0) {
        this.value = initialValue;
    }
} // class Variable

/* --- *
 * P l a n n e r
 * --- */
public class Planner {
    public function Planner():void {
    }
    
    public function incrementalRemove(c:Constraint):void {
		print("Incremental removing");
    }
} // class Planner

function projectionTest():void {
    planner = new Planner();
	var v:Variable = new Variable(100);

    var edit:Constraint = new Constraint();
    var edits:OrderedCollection = new OrderedCollection();
    edits.add(edit);

    for (var i:int = 0; i < 10; i++) {
        v.value = 50;
    }

	print("Destroying constraint");
    edit.destroyConstraint();

}

var planner:Planner = null;
projectionTest();

} // package
