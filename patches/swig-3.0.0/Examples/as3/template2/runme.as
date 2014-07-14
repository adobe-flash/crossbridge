package SWIGExample {

    import avmplus.System;
    import ExampleLib;
    import intList;

    public function ASMain() {
        // Succeed if compile was successful
	Trace(ExampleModule.square(2));
        Trace("Pass");
        return 0;
    }
}
