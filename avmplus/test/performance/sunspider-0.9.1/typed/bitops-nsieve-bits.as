// The Great Computer Language Shootout
//  http://shootout.alioth.debian.org
//
//  Contributed by Ian Osgood

package {

function pad(n:uint,width:uint):String {
  var s:String = n.toString();
  while (s.length < width) s = ' ' + s;
  return s;
}

function primes(isPrime:Array, n:uint) {
  var i:uint, count:uint = 0, m:uint = 10000<<n, size:uint = m+31>>5;

  for (i=0; i<size; i++) isPrime[i] = 0xffffffff;

  for (i=2; i<m; i++)
    if (isPrime[i>>5] & 1<<(i&31)) {
      for (var j:uint=i+i; j<m; j+=i)
        isPrime[j>>5] &= ~(1<<(j&31));
      count++;
    }
}

function sieve():void {
    for (var i:uint = 4; i <= 4; i++) {
        var isPrime:Array = new Array((10000<<i)+31>>5);
        primes(isPrime, i);
    }
}

// main entry point for running testcase
function runTest():void{
sieve();
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}
