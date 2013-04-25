/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org/
   contributed by Isaac Gouy */

package {
function fannkuch(n:uint) {
   var check:uint = 0;
   var perm:Array = new Array(n);
   var perm1:Array = new Array(n);
   var count:Array = new Array(n);
   var maxPerm:Array = new Array(n);
   var maxFlipsCount:uint = 0;
   var m:uint = n - 1;

   for (var i:uint = 0; i < n; i++) perm1[i] = i;
   var r:uint = n;

   while (true) {
      // write-out the first 30 permutations
      if (check < 30){
         var s:String = "";
         for(var i:uint=0; i<n; i++) s += (perm1[i]+1).toString();
         check++;
      }

      while (r != 1) { count[r - 1] = r; r--; }
      if (!(perm1[0] == 0 || perm1[m] == m)) {
         for (var i:uint = 0; i < n; i++) perm[i] = perm1[i];

         var flipsCount:uint = 0;
         var k:uint;

         while (!((k = perm[0]) == 0)) {
            var k2 = (k + 1) >> 1;
            for (var i:uint = 0; i < k2; i++) {
               var temp:uint = perm[i]; perm[i] = perm[k - i]; perm[k - i] = temp;
            }
            flipsCount++;
         }

         if (flipsCount > maxFlipsCount) {
            maxFlipsCount = flipsCount;
            for (var i:uint = 0; i < n; i++) maxPerm[i] = perm1[i];
         }
      }

      while (true) {
         if (r == n) return maxFlipsCount;
         var perm0:uint = perm1[0];
         var i:uint = 0;
         while (i < r) {
            var j:uint = i + 1;
            perm1[i] = perm1[j];
            i = j;
         }
         perm1[r] = perm0;

         count[r] = count[r] - 1;
         if (count[r] > 0) break;
         r++;
      }
   }
}
    
// main entry point for running testcase
function runTest():void{
var n:uint = 8;
var ret:uint = fannkuch(n);
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}