package SWIGExample {

    import avmplus.System;

    public function ASMain() {
        // Object creation
        Trace("Creating some objects:");
        var c = Circle.create(10);
        Trace("Created circle " + c);
        var s = Square.create(10);
        Trace("Created square " + s);

        // Access a static member
        Trace("\nA total of " + + Shape.nshapes + " shapes were created");
        
        // Member data access

        // Set the location of the object
        c.x = 20;
        c.y = 30;
        s.y = -10;
        s.y = 5;

        Trace("\nHere is their current position:");
        Trace("    Circle = (" + c.x + ", " + c.y + ")");
        Trace("    Square = (" + s.x + ", " + s.y + ")");

        // Call some methods
        Trace("\nHere are some properties of the shapes:");
        for each (var o in [c, s]) {
            Trace("    " + o);
            Trace("        area      = " + o.area());
            Trace("        perimeter = " + o.perimeter());
        }
        
        Trace("\nGuess I'll clean up now");
        
        // Note: this invokes the virtual destructor
        c.destroy();
        s.destroy();

        Trace(Shape.nshapes + " shapes remain");
        Trace("Goodbye");

        return 0;
    }
}
