var size:int;

var RANDOM_SEED:int = 10101010;

var datasizes_M:Vector.<int> = new Vector.<int>(4,true);
datasizes_M[0] = 50000;
datasizes_M[1] = 100000;
datasizes_M[2] = 500000;
datasizes_M[3] = 3125;

var datasizes_N:Vector.<int> = new Vector.<int>(4,true);
datasizes_N[0] = 50000;
datasizes_N[1] = 100000;
datasizes_N[2] = 500000;
datasizes_N[3] = 3125;

var datasizes_nz:Vector.<int> = new Vector.<int>(4,true);
datasizes_nz[0] = 250000;
datasizes_nz[1] = 500000;
datasizes_nz[2] = 2500000;
datasizes_nz[3] = 15625;

var SPARSE_NUM_ITER:int = 200;

//Random R = new Random(RANDOM_SEED);
var R:int = RANDOM_SEED;
var x:Vector.<Number>;

var y:Vector.<Number>;

var val:Vector.<Number>;

var col:Vector.<int>;

var row:Vector.<int>;

var ytotal:Number=0.0;
var lastRandom:int=RANDOM_SEED;
if (CONFIG::desktop) {
    var start = new Date();
    JGFrun(0);
    var elapsed = new Date() - start;
}
else { // mobile
    var start = getTimer();
    JGFrun(3);
    var elapsed = getTimer() - start;
}
if (JGFvalidate())
    print("metric time "+elapsed);
else
    print("validation failed");


function _randomInt():int
{
  lastRandom = (lastRandom * 214013 + 2531011) % 16777216;
  return lastRandom;
}
function _randomDouble():Number
{
  return _randomInt()/16777216;
}
function JGFsetsize(sizel:int):void {
  size = sizel;
}

function JGFinitialise():void {

  x = RandomVector(datasizes_N[size], R);
  y = new Vector.<Number>(datasizes_M[size],true);
  
  val = new Vector.<Number>(datasizes_nz[size],true);
  col = new Vector.<int>(datasizes_nz[size],true);
  row = new Vector.<int>(datasizes_nz[size],true);

  for (var i:int = 0; i < datasizes_nz[size]; i++) {

    // generate random row index (0, M-1)
    row[i] = Math.abs(_randomInt()) % datasizes_M[size];

    // generate random column index (0, N-1)
    col[i] = Math.abs(_randomInt()) % datasizes_N[size];

    val[i] = _randomDouble();

  }

}

function JGFkernel():void {

  test(y, val, row, col, x, SPARSE_NUM_ITER);

}

function JGFvalidate():Boolean {

  var refval:Vector.<Number> = new Vector.<Number>(4,true);
  refval[0] = 75.16427500310363;//75.02484945753453;
  refval[1] = 149.5502067152101;
  refval[2] = 749.5245870753752;
  refval[3] = 4.58385725523243;
  var dev:Number = Math.abs(ytotal - refval[size]);
  if (dev > 1.0e-12) {
    print("Validation failed");
    print("ytotal = " + ytotal + "  " + dev + "  " + size);
    return false;
  }else
  {
    print("Validation passed");
    print("ytotal = " + ytotal + "  " + dev + "  " + size);
  }
  print("Y total: " + ytotal + " Dev: " + dev );
  return true;

}

function JGFtidyup():void {
  System.gc();
}

function JGFrun(sizel:int):void {
  JGFsetsize(sizel);
  JGFinitialise();
  JGFkernel();
  JGFvalidate();
  //JGFtidyup();
}

function RandomVector( N:int, R:Number):Vector.<Number> {
  var A:Vector.<Number> = new Vector.<Number>(N,true);

  for (var i:int = 0; i < N; i++)
  {
    A[i] = _randomDouble() * 1e-6;
  }
  return A;
}

  function test( y:Vector.<Number>,  val:Vector.<Number>,  row:Vector.<int>,
                 col:Vector.<int>,  x:Vector.<Number>,  NUM_ITERATIONS:int):void {
  print("Entering test");
  var nz:int = val.length;

  for (var reps:int = 0; reps < NUM_ITERATIONS; reps++) {
    for (var i:int = 0; i < nz; i++) {
      y[row[i]] += x[col[i]] * val[i];
      //print("y is: "+y[row[i]]+"-x is: "+x[col[i]]+"-val is: "+val[i]+"-row is: "+row[i]+"-col is: "+col[i]);
    }
  }

  for (var i:int = 0; i < nz; i++) {
    ytotal += y[row[i]];
  }
  print("leaving test: "+ytotal);
}
