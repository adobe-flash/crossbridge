package SWIGExample {
    import avmplus.System;
    public function ASMain() {
        // Call some templated functions
        Trace(ExampleModule.maxint(3, 7));
        Trace(ExampleModule.maxdouble(3.14, 2.18));

        // Create some class
        var iv = Vecint.create(100);
        var dv = Vecdouble.create(1000);

        for (var i:int = 0; i < 100; i++) {
            iv.setitem(i, 2 * i);
        }

        for (i = 0; i < 1000; i++) {
            dv.setitem(i, 1.0/(i + 1));
        }

        var sum = 0;
        for (i = 0; i < 100; i++) {
            sum = sum + iv.getitem(i);
        }

        Trace(sum);

        sum = 0;
        for (i = 0; i < 1000; i++) {
            sum = sum + dv.getitem(i);
        }
        Trace(sum);

        return 0;
    }
}
