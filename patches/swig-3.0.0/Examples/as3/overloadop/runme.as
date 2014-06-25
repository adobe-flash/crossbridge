package SWIGExample {

    import avmplus.System;

    public function ASMain() {
        // Object creation
        Trace("Creating some objects:");
        var c= Circle.create(10);
        Trace("Created circle " + c);
        var s= Square.create(10);
        Trace("Created square " + s);


        Trace("circle radius / 2: " + c.divop(2));
        Trace("square width / 3 : " + s.divop(3));
        
        // Note: this invokes the virtual destructor
        c.destroy();
        s.destroy();

        return 0;
    }
}
