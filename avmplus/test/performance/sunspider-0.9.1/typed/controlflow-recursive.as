// The Computer Language Shootout
// http://shootout.alioth.debian.org/
// contributed by Isaac Gouy

package {

function ack(m:uint,n:uint):uint{
   if (m==0) { return n+1; }
   if (n==0) { return ack(m-1,1); }
   return ack(m-1, ack(m,n-1) );
}

function fib(n:Number):Number {
    if (n < 2){ return 1; }
    return fib(n-2) + fib(n-1);
}

function tak(x:int,y:int,z:int):int {
    if (y >= x) return z;
    return tak(tak(x-1,y,z), tak(y-1,z,x), tak(z-1,x,y));
}

// main entry point for running testcase
function runTest():void{
for ( var i:uint = 3; i <= 5; i++ ) {
    ack(3,i);
    fib(17.0+i);
    tak(3*i+3,2*i+2,i+1);
}
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}
