package SWIGExample {
    import avmplus.System;
    import com.adobe.flascc.CModule;

    public function ASMain() {
        
		// note that the SWIG file ignores this input from ActionScript 
		// and passes the value of a static ByteArray member to C instead
        ExampleModule.printNumber(1111);
        ExampleModule.printNumber2(2222);
        
        return 0;
    }

}

