package SWIGExample {
    import avmplus.System;

    public function ASMain() {
        // Object creation

        // Print out the values of some enums
        Trace("*** color ***");
        Trace("    RED    = " + ExampleModule.RED);
        Trace("    BLUE   = " + ExampleModule.BLUE);
        Trace("    GREEN  = " + ExampleModule.GREEN);

        Trace("\n*** Foo:speed ***");
        Trace("    Foo_IMPULSE   = " + Foo.IMPULSE);
        Trace("    Foo_WARP      = " + Foo.WARP);
        Trace("    Foo_LUDICROUS = " + Foo.LUDICROUS);

        Trace("\nTesting use of enums with functions\n");

        ExampleModule.enum_test(ExampleModule.RED, Foo.IMPULSE);
        ExampleModule.enum_test(ExampleModule.BLUE, Foo.WARP);
        ExampleModule.enum_test(ExampleModule.GREEN, Foo.LUDICROUS);
        ExampleModule.enum_test(1234, 5678);

        Trace("\nTesting use of enum with class methods");
        var f = Foo.create();
        
        f.enum_test(Foo.IMPULSE);
        f.enum_test(Foo.WARP);
        f.enum_test(Foo.LUDICROUS);
    
        return 0;
    }
}
