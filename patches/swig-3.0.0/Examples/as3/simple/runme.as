package SWIGExample {
    import avmplus.System;
    public function ASMain() {
        var x = 42, y = 105;
        var g = ExampleModule.gcd(x, y);
        Trace('The gcd of ' + x + ' and ' + y + ' is ' + g);
        
        Trace('Foo = ' + ExampleModule.Foo);
        ExampleModule.Foo = 3.1415926;
        Trace('Foo = ' + ExampleModule.Foo);
    
        return 0;
    }

}

