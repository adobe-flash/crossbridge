// The Great Computer Language Shootout
//  http://shootout.alioth.debian.org
//
//  Contributed by Ian Osgood

package {

public class frequency {
    public var c:String;
    public var p:Number;
    public function frequency(c:String, p:Number) {
        this.c = c;
        this.p = p;
    }
}

var last:int = 42, A:int = 3877, C:int = 29573, M:int = 139968;

function rand(max:int):Number {
  last = (last * A + C) % M;
  return max * last / M;
}

var ALU:String =
  "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
  "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
  "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
  "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
  "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
  "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
  "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

var IUB:Array = new Array();
IUB.push(new frequency("a", 0.27));
IUB.push(new frequency("c", 0.12));
IUB.push(new frequency("g", 0.12));
IUB.push(new frequency("t", 0.27));
IUB.push(new frequency("B", 0.02));
IUB.push(new frequency("D", 0.02));
IUB.push(new frequency("H", 0.02));
IUB.push(new frequency("K", 0.02));
IUB.push(new frequency("M", 0.02));
IUB.push(new frequency("N", 0.02));
IUB.push(new frequency("R", 0.02));
IUB.push(new frequency("S", 0.02));
IUB.push(new frequency("V", 0.02));
IUB.push(new frequency("W", 0.02));
IUB.push(new frequency("Y", 0.02));


var HomoSap:Array = new Array();
HomoSap.push(new frequency("a", 0.3029549426680));
HomoSap.push(new frequency("c", 0.1979883004921));
HomoSap.push(new frequency("g", 0.1975473066391));
HomoSap.push(new frequency("t", 0.3015094502008));

function makeCumulative(table:Array):void {
    var cp:Number = 0.0;
    for (var i:int = 0; i < table.length; i++) {
        cp += table[i].p;
        table[i].p = cp;
    }
}

function fastaRepeat(n:int, seq:String):String {
  var seqi:int = 0, lenOut:int = 60;
  var ret:String;
  while (n>0) {
    if (n<lenOut) lenOut = n;
    if (seqi + lenOut < seq.length) {
      ret = seq.substring(seqi, seqi+lenOut);
      seqi += lenOut;
    } else {
      var s:String = seq.substring(seqi);
      seqi = lenOut - s.length;
      ret = s + seq.substring(0, seqi);
    }
    n -= lenOut;
  }
  return ret;
}

function fastaRandom(n:int, table:Array):String {
  var line:Array = new Array(60);
  var ret:String;
  makeCumulative(table);
  while (n>0) {
    if (n<line.length) line = new Array(n);
    for (var i:int=0; i<line.length; i++) {
      var r:Number = rand(1);
      for (var c:int=0; c < table.length; c++){
        if (r < table[c].p) {
          line[i] = table[c].c;
          break;
        }
      }
    }
    ret = line.join('');
    n -= line.length;
  }
  return ret;
}

// main entry point for running testcase
function runTest():void{
var ret:String;
var count:uint = 7;
ret = fastaRepeat(2*count*100000, ALU);
ret = fastaRandom(3*count*1000, IUB);
ret = fastaRandom(5*count*1000, HomoSap);

} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}
