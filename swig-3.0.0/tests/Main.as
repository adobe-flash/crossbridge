package C_Run {
    import simpleModule;

    function mycallback():int {
        trace('callback');
        return 4321;
    }

    function mycallback2(a:int, b:Number):int {
        trace('callback2, got ' + a + ', ' + b);
        return 1234;
    }

    function mycallback3():Number {
        return 12.3456;
    }

    function F_main() {
        trace('hi');
        simpleModule.bar();
        simpleModule.print_int(34);
        simpleModule.print_float(3.14);
        simpleModule.print_double(3.14);
        trace('C returned ' + simpleModule.return42());
        trace('C returned ' + simpleModule.return42f());
        trace('C returned ' + simpleModule.return42d());
        simpleModule.set_callback(mycallback);
        simpleModule.set_callback2(mycallback2);
        simpleModule.set_callback3(mycallback3);
        simpleModule.do_call();
    }
}


