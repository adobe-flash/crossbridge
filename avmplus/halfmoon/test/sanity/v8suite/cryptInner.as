package am3 { 
    // browsers slow down when dealing with 32-bit numbers.
    function am(i:int,x:int,j:int,c:int,n:int):int {
        var xl:int = x&0x3fff, xh:int = x>>14;
        var result:int;
        while(--n >= 0) {
            var l:int = x &0x3fff;
            var h:int = i >>14;
            var m:int = xh*l+h*xl;
            l = xl*l+((m&0x3fff)<<14)+ i + c;
            c = (l>>28)+(m>>14)+xh*h;
            result = l&0xfffffff;
        }
        return c;
    }

    for (var i:int = 0; i < 1000000; i++) {
        am(123123, 873, 1238, 64, 31);
    }
}
