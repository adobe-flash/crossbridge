package SWIGExample {
import avmplus.System;

public function ASMain() {
    Trace("ICONST = " + ExampleModule.ICONST + " (should be 42)");
    Trace("FCONST = " + ExampleModule.FCONST + " (should be 2.1828)");
    Trace("CCONST = " + ExampleModule.CCONST + " (should be 'x')");
    Trace("CCONST = " + ExampleModule.CCONST2 + "(this should be on a new line)");
    Trace("SCONST = " + ExampleModule.SCONST + " (should be 'Hello World')");
    Trace("SCONST2 = " + ExampleModule.SCONST2 + " (should be '\"Hello World\"')");
    Trace("EXPR = " + ExampleModule.EXPR + " (should be 48.5484)");
    Trace("iconst = " + ExampleModule.iconst + " (should be 37)");
    Trace("fconst = " + ExampleModule.fconst + " (should be 3.14)");
    Trace("b = " + ExampleModule.b + " (should be 42)");

    Trace("EXTERN = " + ExampleModule.EXTERN + " (should be undefined)");
    Trace("FOO = " + ExampleModule.FOO + " (should be undefined)");

    return 0;
}
}

