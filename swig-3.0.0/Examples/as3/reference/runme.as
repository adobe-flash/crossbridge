package SWIGExample {
    import avmplus.System;
    public function ASMain() {
        // Object creation
        Trace("Creating some objects");
        var a = CVector.create(3, 4, 5);
        var b = CVector.create(10, 11, 12);

        Trace("    Created " + a.cprint());
        Trace("    Created " + b.cprint());
        
        // Call an overloaded operator

        Trace("Adding a+b");
        var c = ExampleModule.addv(a.swigCPtr, b.swigCPtr);
        Trace("    a+b = " + c.cprint());

        return 0;
    }
}

