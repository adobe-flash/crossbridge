var size:int;

  var datasizes:Vector.<int> = new Vector.<int>(4,true);
  datasizes[0] = 1000;
  datasizes[1] = 1500;
  datasizes[2] = 2000;
  datasizes[3] = 225;

  var JACOBI_NUM_ITER:int = 100;

  var RANDOM_SEED:int = 10101010;
  var R:int=RANDOM_SEED;
  var Gtotal:Number = 0.0;
  var lastRandom:int=RANDOM_SEED;
  function _randomInt():int
  {
    lastRandom = (lastRandom * 214013 + 2531011) % 16777216;
    return lastRandom;
  }
  function _randomDouble():Number
  {
    return _randomInt()/16777216;
  }
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
    
  function JGFsetsize(sizel:int):void {
    size = sizel;
  }

  function JGFinitialise():void {

  }

  function JGFkernel():void {

    var G:Vector.<Vector.<Number>> = RandomMatrix(datasizes[size], datasizes[size], R);

    SORrun(1.25, G, JACOBI_NUM_ITER);

  }

  function JGFvalidate():Boolean {

    var refval:Vector.<Number> = new Vector.<Number>(4,true);
    refval[0] = 0.49819968382576163; //0.4984199298207158;
    refval[1] = 1.122043587235093;
    refval[2] = 1.9967774998523777;
    refval[3] = 0.024878259186755635
    var dev:Number = Math.abs(Gtotal - refval[size]);
    if (dev > 1.0e-12) {
      print("Validation failed");
      print("Gtotal = " + Gtotal + "  " + dev + "  " + size);
      return false;
    }
    print("Gtotal is: " + Gtotal);
    print("Dev is: " + dev);
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

  function RandomMatrix(M:int,  N:int, R:int):Vector.<Vector.<Number>> {
    var A:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(M,true); //[][] = new double[M][N];
    for(var i:int =0;i<N;i++)
    {
      A[i]=new Vector.<Number>(N,true);
    }
    for (var i:int = 0; i < N; i++)
      for (var j:int = 0; j < N; j++) {
        A[i][j] = _randomDouble() * 1e-6;
      }
    return A;
  }
  
  function SORrun( omega:Number,  G:Vector.<Vector.<Number>>,  num_iterations:int):void {
    var M:int = G.length;
    var N:int = G[0].length;

    var omega_over_four:Number = omega * 0.25;
    var one_minus_omega:Number = 1.0 - omega;

    // update interior points
    //
    var Mm1:int = M - 1;
    var Nm1:int = N - 1;


    for (var p:int = 0; p < num_iterations; p++) {
      for (var i:int = 1; i < Mm1; i++) {
        var Gi:Vector.<Number> = G[i];
        var Gim1:Vector.<Number> = G[i - 1];
        var Gip1:Vector.<Number> = G[i + 1];
        for (var j:int = 1; j < Nm1; j++)
          Gi[j] = omega_over_four
              * (Gim1[j] + Gip1[j] + Gi[j - 1] + Gi[j + 1])
              + one_minus_omega * Gi[j];
      }
    }

    for (var i:int = 1; i < Nm1; i++) {
      for (var j:int = 1; j < Nm1; j++) {
        Gtotal += G[i][j];
      }
    }

  }