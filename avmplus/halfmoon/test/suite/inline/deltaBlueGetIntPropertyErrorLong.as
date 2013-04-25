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
		print("OrderedCollection::add Adding constraint");
		print("Length: " + elms.length);
        this.elms[elms.length] = elm;
		print("Finished adding constraint");
    }
    
    public function at(index:int):* {
		print("Atting : " + index);
        var result = this.elms[index];
		print(result);
		return result;
    }
    
    public function size():int {
        return this.elms.length;
    }
    
    public function removeFirst():*{
		print("remove");
        return this.elms.pop();
    }
    
} // class OrderedCollection


public class Strength {
    public var strengthValue:int;
    public var name:String;

    // Strength constants.
    static public const REQUIRED:Strength        = new Strength(0, "required");
    static public const STONG_PREFERRED:Strength = new Strength(1, "strongPreferred");
    static public const PREFERRED:Strength       = new Strength(2, "preferred");
    static public const STRONG_DEFAULT:Strength  = new Strength(3, "strongDefault");
    static public const NORMAL:Strength          = new Strength(4, "normal");
    static public const WEAK_DEFAULT:Strength    = new Strength(5, "weakDefault");
    static public const WEAKEST:Strength         = new Strength(6, "weakest");
    
    public function Strength(strengthValue:int, name:String):void {
        this.strengthValue = strengthValue;
        this.name = name;
    }
    
    static public function stronger(s1:Strength, s2:Strength):Boolean {
        return s1.strengthValue < s2.strengthValue;
    }
    
    static public function weaker(s1:Strength, s2:Strength):Boolean {
        return s1.strengthValue > s2.strengthValue;
    }
    
    static public function weakestOf(s1:Strength, s2:Strength):Strength {
        return weaker(s1, s2) ? s1 : s2;
    }
    
} // class Strength


public class Constraint {
    public var strength:Strength;
    
    public function Constraint(strength:Strength):void {
		print("in constraint super");
        this.strength = strength;
		print("finished constraint super");
    }
    
    /**
     * Activate this constraint and attempt to satisfy it.
     */
    public function addConstraint():void {
		print("Constraint::addConstraint");
        this.addToGraph();
		print("Finished adding to graph in Constraint::addConstraint");
        planner.incrementalAdd(this);
		print("Finished planner.incremental add in Constraint::addConstraint");
    }
    
    
    // functions to be overridden by children
    public function addToGraph():void {}
    public function output():Variable {}
    public function chooseMethod(mark:int):void {}
    public function isSatisfied():Boolean {return false;}
    public function markInputs(mark:int):void {}
    public function markUnsatisfied():void {}
    public function inputsKnown(mark:int):Boolean {return false;}
    public function recalculate():void {}
    public function execute():void {}
    
    /**
     * Attempt to find a way to enforce this constraint. If successful,
     * record the solution, perhaps modifying the current dataflow
     * graph. Answer the constraint that this constraint overrides, if
     * there is one, or nil, if there isn't.
     * Assume: I am not already satisfied.
     */
    public function satisfy(mark:int):Constraint {
        this.chooseMethod(mark);
        if (!this.isSatisfied()) {
            if (this.strength == Strength.REQUIRED)
                print("Could not satisfy a required constraint!");
            return null;
        }
        this.markInputs(mark);
        var out:Variable = this.output();
        var overridden:Constraint = out.determinedBy;
        if (overridden != null) overridden.markUnsatisfied();
        out.determinedBy = this;
        if (!planner.addPropagate(this, mark)) {
            print("Cycle encountered");
		}

        out.mark = mark;
        return overridden;
    }
    
    public function isInput():Boolean {
        return false;
    }

} // class Constraint


public class Direction {
    static public const NONE:int     = 0;
    static public const FORWARD:int  = 1;
    static public const BACKWARD:int = -1;
    
} // class Direction

public class BinaryConstraint extends Constraint {
    protected var v1:Variable;
    protected var v2:Variable;
    protected var direction:int;
    
    public function BinaryConstraint(var1:Variable, var2:Variable, strength:Strength) {
		print("Starting binary super");
        super(strength);
        this.v1 = var1;
        this.v2 = var2;
        this.direction = Direction.NONE;
		print("Adding constraint from binary constraint constructor");
        this.addConstraint();
		print("Finished binary constructor");
    }
    
    //BinaryConstraint.inherits(Constraint);
    
    /**
     * Decides if this constratint can be satisfied and which way it
     * should flow based on the relative strength of the variables related,
     * and record that decision.
     */
    override public function chooseMethod(mark:int):void {
        if (this.v1.mark == mark) {
            this.direction = (this.v1.mark != mark && Strength.stronger(this.strength, this.v2.walkStrength))
                ? Direction.FORWARD
                : Direction.NONE;
        }
        if (this.v2.mark == mark) {
            this.direction = (this.v1.mark != mark && Strength.stronger(this.strength, this.v1.walkStrength))
                ? Direction.BACKWARD
                : Direction.NONE;
        }
        if (Strength.weaker(this.v1.walkStrength, this.v2.walkStrength)) {
            this.direction = Strength.stronger(this.strength, this.v1.walkStrength)
                ? Direction.BACKWARD
                : Direction.NONE;
        } else {
            this.direction = Strength.stronger(this.strength, this.v2.walkStrength)
                ? Direction.FORWARD
                : Direction.BACKWARD
        }
    }
    
    /**
     * Add this constraint to the constraint graph
     */
    override public function addToGraph():void {
		print("BinaryConstraint::Adding to graph");
        this.v1.addConstraint(this);
		print("V2 add constraint");
        this.v2.addConstraint(this);
        this.direction = Direction.NONE;
    }
    
    /**
     * Answer true if this constraint is satisfied in the current solution.
     */
    override public function isSatisfied():Boolean {
        return this.direction != Direction.NONE;
    }
    
    /**
     * Mark the input variable with the given mark.
     */
    override public function markInputs(mark:int):void {
        this.input().mark = mark;
    }
    
    /**
     * Returns the current input variable
     */
    public function input():Variable {
        return (this.direction == Direction.FORWARD) ? this.v1 : this.v2;
    }
    
    /**
     * Returns the current output variable
     */
    override public function output():Variable {
        return (this.direction == Direction.FORWARD) ? this.v2 : this.v1;
    }
    
    /**
     * Calculate the walkabout strength, the stay flag, and, if it is
     * 'stay', the value for the current output of this
     * constraint. Assume this constraint is satisfied.
     */
    override public function recalculate():void {
        var ihn:Variable = this.input(), out:Variable = this.output();
        out.walkStrength = Strength.weakestOf(this.strength, ihn.walkStrength);
        out.stay = ihn.stay;
        if (out.stay) this.execute();
    }
    
    /**
     * Record the fact that this constraint is unsatisfied.
     */
    override public function markUnsatisfied():void {
        this.direction = Direction.NONE;
    }
    
    override public function inputsKnown(mark:int):Boolean {
        var i:Variable = this.input();
        return i.mark == mark || i.stay || i.determinedBy == null;
    }
    
} // class BinaryConstraint

/* --- *
 * V a r i a b l e
 * --- */
public class Variable {
    /**
     * A constrained variable. In addition to its value, it maintain the
     * structure of the constraint graph, the current dataflow graph, and
     * various parameters of interest to the DeltaBlue incremental
     * constraint solver.
     **/
    public var value:int;
    public var constraints:OrderedCollection;
    public var determinedBy:Constraint;
    public var mark:int;
    public var walkStrength:Strength;
    public var stay:Boolean;
    public var name:String;
    
    function Variable(name:String, initialValue:int = 0) {
        this.value = initialValue;
        this.constraints = new OrderedCollection();
        this.determinedBy = null;
        this.mark = 0;
        this.walkStrength = Strength.WEAKEST;
        this.stay = true;
        this.name = name;
    }
    
    /**
     * Add the given constraint to the set of all constraints that refer
     * this variable.
     */
    public function addConstraint(c:Constraint):void {
		print("Variable::addConstraint");
        this.constraints.add(c);
		print("Finished var::addConstraint");
    }
} // class Variable

/* --- *
 * P l a n n e r
 * --- */
public class Planner {
    public var currentMark:int;
    
    public function Planner():void {
        this.currentMark = 0;
    }
    
    public function incrementalAdd(c:Constraint):void {
		print("Starting incremental add");
        var mark:int = this.newMark();
        var overridden:Constraint = c.satisfy(mark);
        while (overridden != null) {
            overridden = overridden.satisfy(mark);
		}

		print("Finished incremental add");
    }
    
    public function newMark():int {
        return ++this.currentMark;
    }
    
    public function addPropagate(c:Constraint, mark:int):Boolean {
		print("adding propagate");
        var todo:OrderedCollection = new OrderedCollection();
        todo.add(c);
        while (todo.size() > 0) {
            var d:Constraint = todo.removeFirst();
            if (d.output().mark == mark) {
                this.incrementalRemove(c);
                return false;
            }

			print("Recalculating");
            d.recalculate();
			print("Finished recalculating in Planner::addPropagate, adding constraints");
            this.addConstraintsConsumingTo(d.output(), todo);
        }
        return true;
    }
    
    
    public function addConstraintsConsumingTo(v:Variable, coll:OrderedCollection) {
        var determining:Constraint = v.determinedBy;
		print("Determing: " + determining);
        var cc:OrderedCollection = v.constraints;
		print("CC ordered collection: " + cc);
		print("CC size is: " + cc.size());
        for (var i:int = 0; i < cc.size(); i++) {
            var c:Constraint = cc.at(i);
			print("Received constraint is: " + c);
            if (c != determining && c.isSatisfied()) {
                coll.add(c);
			}
        }
    }

} // class Planner

/* --- *
 * P l a n
 * --- */

public class Plan {
    /**
     * A Plan is an ordered list of constraints to be executed in sequence
     * to resatisfy all currently satisfiable constraints in the face of
     * one or more changing inputs.
     */
    public var v:OrderedCollection;
    
    public function Plan():void {
        this.v = new OrderedCollection();
    }
    
    public function addConstraint(c:Constraint):void {
        this.v.add(c);
    }
    
    public function size():int {
        return this.v.size();
    }
    
    
    public function execute():void {
        for (var i:int = 0; i < this.size(); i++) {
            var c:Constraint = this.constraintAt(i);
            c.execute();
        }
    }

} // class Plan

function chainTest(n:int) {
    planner = new Planner();
    var prev:Variable = null, first:Variable = null, last:Variable = null;

	var ordered:OrderedCollection = new OrderedCollection();
	var constraint:Constraint = new Constraint(Strength.REQUIRED);
	ordered.add(constraint);
	var retrive:Constraint = ordered.at(0);


/*
	var name:String = "v10";
	var secondName:String = "v20";
	prev = new Variable(name);
	first = new Variable(secondName);
	new BinaryConstraint(prev, first, Strength.REQUIRED);
*/
	
}

// Global variable holding the current planner.
var planner:Planner = null;

function deltaBlue():void {
    chainTest(100);
}


deltaBlue();

} // package
