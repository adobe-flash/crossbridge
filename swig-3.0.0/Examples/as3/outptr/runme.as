package SWIGExample {
    import avmplus.System;
    public function ASMain() {
		
		var v:Vector.<int> = new Vector.<int>();
		v.push(1);
		v.push(2);
		v.push(3);
		v.push(4);
		v.push(5);
		
		// pass an ActionScript object in and get an ActionScript object returned
		var reverse:Vector.<int> = ExampleModule.reverseArray(v); 
		Trace("Reversed: " + reverse + " == 5,4,3,2,1");
		
		var increment:Vector.<int> = ExampleModule.incrementArray(v); 
		Trace("Incremented: " + increment + " == 2,3,4,5,6");
   
        return 0;
    }

}

