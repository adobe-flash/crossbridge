var v = "not initialized";

function f() {
    return 37;
}

class C {
    static var v;
    static {
        v = f();  // class initializer, "static { ... }" in Java
    }
    
    var w;

    function C() {
        w = v;
    }
}

print("ivar lifted from static: " + (new C()).w);
print(v);
print("DONE");
