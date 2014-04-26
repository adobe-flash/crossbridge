package SWIGExample {
    import avmplus.System;
    public function ASMain() {
        
        Trace('a = ' + ExampleModule.a);
        ExampleModule.a = 31415926;
        Trace('a = ' + ExampleModule.a);
    
        Trace('b = ' + ExampleModule.b);
        ExampleModule.b = 123;
        Trace('b = ' + ExampleModule.b);
        
        Trace('c = ' + ExampleModule.c);
        ExampleModule.c = 'a';
        Trace('c = ' + ExampleModule.c);
        
        Trace('d = ' + ExampleModule.d);
        ExampleModule.d = 'b';
        Trace('d = ' + ExampleModule.d);
        
        Trace('e = ' + ExampleModule.e);
        ExampleModule.e = 321;
        Trace('e = ' + ExampleModule.e);
        
        return 0;
    }

}

