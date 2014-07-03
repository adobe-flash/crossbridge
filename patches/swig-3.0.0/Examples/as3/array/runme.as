package SWIGExample {
    import avmplus.System;
    import com.adobe.flascc.CModule;
    import flash.utils.ByteArray;

    public function ASMain() {
  
        var d = new DoublesValue();
        var dptr = CModule.malloc(DoublesValue.size);

        var doubles = new ByteArray();
        doubles.endian = "littleEndian";
        for (var i = 0; i < 10; i++) {
            doubles.writeDouble(1 + i / 2);
        }
        doubles.position = 0;
        d.array_member = doubles;
        d.write(dptr);
        
        ExampleModule.print_doubles(dptr);

        CModule.free(dptr);

        var c = new CharsValue();
        var cptr = CModule.malloc(CharsValue.size);

        var chars = new ByteArray();
        for (var i = 0; i < 10; i++) {
            chars.writeByte(65 + i);
        }
        chars.position = 0;
        c.char_member = chars;
        c.write(cptr);
        ExampleModule.print_chars(cptr);

        CModule.free(cptr);

        return 0;
    }

}

