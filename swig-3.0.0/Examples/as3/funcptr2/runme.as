package SWIGExample {
import avmplus.System;

public function ASMain() {
    var a = 37;
    var b = 42;

    // Now call our C function with a bunch of callbacks
    Trace("Trying some C callback functions");
    Trace("    a        = " + a);
    Trace("    b        = " + b);
    Trace("    ADD(a,b) = " + ExampleModule.do_op(a, b, ExampleModule.ADD));
    Trace("    SUB(a,b) = " + ExampleModule.do_op(a, b, ExampleModule.SUB));
    Trace("    MUL(a,b) = " + ExampleModule.do_op(a, b, ExampleModule.MUL));

    Trace("Here is what the C callback function objects look like in AS");
    Trace("    ADD      = " + ExampleModule.ADD);
    Trace("    SUB      = " + ExampleModule.SUB);
    Trace("    MUL      = " + ExampleModule.MUL);

    Trace("Call the wrappers directly...");
    Trace("    add(a,b) = " + ExampleModule.add(a, b));
    Trace("    sub(a,b) = " + ExampleModule.sub(a, b));

    Trace("Call the Function objects directly...");
    Trace("    add(a,b) = " + ExampleModule.ADD(a, b));
    Trace("    sub(a,b) = " + ExampleModule.SUB(a, b));

    return 0;
}
}
