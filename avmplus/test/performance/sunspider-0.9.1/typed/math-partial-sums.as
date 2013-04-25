// The Computer Language Shootout
// http://shootout.alioth.debian.org/
// contributed by Isaac Gouy

package {

function partial(n:uint){
    var a1:Number,a2:Number,a3:Number,a4:Number,a5:Number,a6:Number,a7:Number,a8:Number,a9:Number;
    a1 = a2 = a3 = a4 = a5 = a6 = a7 = a8 = a9 = 0.0;
    var twothirds:Number = 2.0/3.0;
    var alt:Number = -1.0;
    var k2:uint = 0, k3:uint = 0;
    var sk:Number = 0.0, ck:Number = 0.0;
    
    for (var k:uint = 1; k <= n; k++){
        k2 = k*k;
        k3 = k2*k;
        sk = Math.sin(k);
        ck = Math.cos(k);
        alt = -alt;
        
        a1 += Math.pow(twothirds,k-1);
        a2 += Math.pow(k,-0.5);
        a3 += 1.0/(k*(k+1.0));
        a4 += 1.0/(k3 * sk*sk);
        a5 += 1.0/(k3 * ck*ck);
        a6 += 1.0/k;
        a7 += 1.0/k2;
        a8 += alt/k;
        a9 += alt/(2*k -1);
    }
}

// main entry point for running testcase
function runTest():void{
for (var i:uint = 1024; i <= 16384; i *= 2) {
    partial(i);
}
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}
