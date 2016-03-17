package SWIGExample {

    import avmplus.System;
    import com.adobe.flascc.CModule;
    import flash.utils.ByteArray;

    public function ASMain() {
        var addr:int = CModule.malloc(BarValue.size);
        var bar = new BarValue(CModule.ram, addr);

        ExampleModule.set_struct_members(addr);
        Trace('(AS) a = ' + bar.a);
        Trace('(AS) b = ' + bar.b);
        Trace('(AS) c = ' + bar.c);
        Trace('(AS) d = ' + bar.d);
        
        var nums:ByteArray = new ByteArray();
        nums.endian = "littleEndian";
        nums.writeInt(111);
        nums.writeInt(222);
        nums.writeInt(333);
        nums.writeInt(444);
        nums.position = 0;
        
        bar.nums = nums;
        
        var outputNums:ByteArray = bar.nums;
        Trace('(AS) nums[0] = ' + outputNums.readInt());
        Trace('(AS) nums[1] = ' + outputNums.readInt());
        Trace('(AS) nums[2] = ' + outputNums.readInt());
        Trace('(AS) nums[3] = ' + outputNums.readInt());

        ExampleModule.print_struct_members(addr);
        bar.a = -2;
        bar.b = 345;
        bar.c = 'b'.charCodeAt(0);
        bar.d = 6.54321;
        ExampleModule.print_struct_members(addr);

        ExampleModule.print_struct_offsets();
        if (bar.addressOf() != addr) {
            Trace('error: addressOf() returned wrong value: ' 
                    + bar.addressOf() + ' (should be ' + addr + ')');
        }

        CModule.free(addr);

        return 0;
    }

}

