package SWIGExample {
    import avmplus.System;
    public function ASMain() {
        var pt = ExampleModule.getpoint();
        Trace('The point is (' + pt.x + ', ' + pt.y + ')');
        return 0;
    }

}

