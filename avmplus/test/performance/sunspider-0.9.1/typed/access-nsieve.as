// The Great Computer Language Shootout
// http://shootout.alioth.debian.org/
//
// modified by Isaac Gouy

package {
    
function pad(number:uint,width:uint):String{
   var s:String = number.toString();
   var prefixWidth:uint = width - s.length;
   if (prefixWidth>0){
      for (var i:uint=1; i<=prefixWidth; i++) s = " " + s;
   }
   return s;
}

function nsieve(m:uint, isPrime:Array):uint{
   var i:uint, k:uint, count:uint;

   for (i=2; i<=m; i++) { isPrime[i] = true; }
   count = 0;

   for (i=2; i<=m; i++){
      if (isPrime[i]) {
         for (k=i+i; k<=m; k+=i) isPrime[k] = false;
         count++;
      }
   }
   return count;
}

function sieve():void {
    for (var i:uint = 1; i <= 3; i++ ) {
        var m:uint = (1<<i)*10000;
        var flags:Array = new Array(m+1);
        nsieve(m, flags);
    }
}

// main entry point for running testcase
function runTest():void{
sieve();
} //runTest()

// warm up run of testcase
runTest();
var startTime:int = new Date().getTime();
runTest();
var time:int = new Date().getTime() - startTime;
print("metric time " + time);

}