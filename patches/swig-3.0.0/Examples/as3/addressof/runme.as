package SWIGExample {
    import avmplus.System;
    import com.adobe.flascc.CModule;

    public function ASMain() {
        
        var addr:int = CModule.malloc(BarValue.size);
        var bar = new BarValue(CModule.ram, addr);
        
        ExampleModule.printPointer(bar.addressOf());
        ExampleModule.printPointer(addr);

        bar.num = 5;
        
        ExampleModule.printPointer(bar.addressOf());
        ExampleModule.printPointer(addr);

        return 0;
    }

}

