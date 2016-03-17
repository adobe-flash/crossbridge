package SWIGExample {
    import avmplus.System;

    function mycallback():int {
        Trace('callback');
        return 4321;
    }

    function mycallback2(a:int, b:Number):int {
        Trace('callback2, got ' + a + ', ' + b);
        return 1234;
    }

    function mycallback3():Number {
        return 12.3456;
    }
    
    public function ASMain() {
        ExampleModule.set_callback(mycallback);
        ExampleModule.set_callback2(mycallback2);
        ExampleModule.set_callback3(mycallback3);
        ExampleModule.do_call();

        return 0;
    }

}

