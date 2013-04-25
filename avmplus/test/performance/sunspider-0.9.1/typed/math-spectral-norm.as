// The Great Computer Language Shootout
// http://shootout.alioth.debian.org/
//
// contributed by Ian Osgood

package {

function A(i:uint,j:uint):Number {
  return 1/((i+j)*(i+j+1)/2+i+1);
}

function Au(u,v):void {
  for (var i:uint=0; i<u.length; ++i) {
    var t:Number = 0;
    for (var j:uint=0; j<u.length; ++j)
      t += A(i,j) * u[j];
    v[i] = t;
  }
}

function Atu(u:Array,v:Array):void {
  for (var i:uint=0; i<u.length; ++i) {
    var t:Number = 0;
    for (var j:uint=0; j<u.length; ++j)
      t += A(j,i) * u[j];
    v[i] = t;
  }
}

function AtAu(u:Array,v:Array,w:Array):void {
  Au(u,w);
  Atu(w,v);
}

function spectralnorm(n:uint):Number {
  var i:uint = 0;
  var u:Array = new Array(), v:Array = new Array(), w:Array = new Array();
  var vv:Number=0, vBv:Number=0;
  for (i=0; i<n; ++i) {
    u[i] = 1; v[i] = w[i] = 0;
  }
  for (i=0; i<10; ++i) {
    AtAu(u,v,w);
    AtAu(v,u,w);
  }
  for (i=0; i<n; ++i) {
    vBv += u[i]*v[i];
    vv  += v[i]*v[i];
  }
  return Math.sqrt(vBv/vv);
}

// main entry point for running testcase
function runTest():void{
for (var i:uint = 6; i <= 48; i *= 2) {
    spectralnorm(i);
}
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}
