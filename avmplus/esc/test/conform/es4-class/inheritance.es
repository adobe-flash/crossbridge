// Sundry aspects of inheritance, maybe too much for one file.
//
// Current bugs:
//  - final methods can be overridden
//  - protected scopes are not opened in subclasses
//  - interface details
//    - checking that class actually does implement
//    - emit abc that contains signatures
//
// Gets the following right:
//  - subclassing
//  - final classes
//  - method override
//  - super.f()
//  - private slots
//  - interface parsing
//
// Appears to catch these problems (not tested here):
//  - subclassing final classes
//  - overridden methods without an "override" clause
//  - reading unknown property from sealed object throws error (should return undefined?)
//  - trying to create property on sealed object throws error (should silently fail?)

var v = "global";
var j2 = "WRONG";

interface I {
    function f();
}

class B implements I {
    function B(x,y) {
        print(x + " " + y);
    }

    function testOverride()
        "base";

    function testInherited()
        v;

    private var j = 10;
    protected var j2 = 20;

    final function testFinal()
        "in B";

    static var v = "B";
}

final class D extends B { 
    function D(x) : super(x,2) {
        print(v);
    }

    function testBaseStatic()
        v;

    function testProtected() 
        j2;

    function testSuperCall() 
        super.testOverride();

    override function testOverride()
        "derived";

    // This should not compile, but it should at least fail in the verifier
    override function testFinal()
        "illegal override in D"; 
}

var d = new D(1);
print(v);
print(d.testOverride());
print(d.testInherited());
print(d.testBaseStatic());
print(d.testProtected());
print('---');
print(d is B);
print(d is D);
print(B.v);
print(D.v);
print(d.testFinal());
print(d.testSuperCall());
print(d is I);
print("DONE");
