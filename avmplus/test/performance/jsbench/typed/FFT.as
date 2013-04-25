var size:int=0;
var datasizes:Vector.<int> = new Vector.<int>(5);
datasizes[0] = 524288;
datasizes[1] = 2097152;
datasizes[2] = 8388608;
datasizes[3] = 16777216;
datasizes[4] = 32768;
var RANDOM_SEED:int = 10101010;
var JDKtotal:Number = 0.0;
var JDKtotali:Number = 0.0;
var lastRandom:int=RANDOM_SEED;

  function _randomInt():int
  {
    lastRandom = (lastRandom * 214013 + 2531011) % 16777216;
    return lastRandom;
  }
  function _randomDouble():Number
  {
    var randomInt:int = _randomInt();
    var result:Number = randomInt/16777216.0;
    if( result < 0 ) {
      print("Got a negative number: " + result + "With random int: " + randomInt );
    }
    return result;
  }
  
if (CONFIG::desktop) {
    var start = new Date();
    JGFrun(0);
    var elapsed = new Date() - start;
}
else { // mobile
    var start = getTimer();
    JGFrun(4);
    var elapsed = getTimer() - start;
}
if (JGFvalidate())
    print("metric time "+elapsed);
else
    print("validation failed");

function JGFrun(sizei):void {
    JGFsetsize(sizei);
    JGFinitialise();
    JGFkernel();
    JGFvalidate();

  }
  
  function JGFsetsize(sizei):void {
    size = sizei;
  }
  
  function JGFinitialise():void {

  }
  
  function JGFkernel():void {

    var x:Vector.<Number> = RandomVector(2 * (datasizes[size]), RANDOM_SEED);
    transform(x);
    inverse(x);

  }
  function JGFvalidate():Boolean {
    var refval:Vector.<Number> = new Vector.<Number>(5);
    refval[0] = 0.5591972812499846;     // 0.4317407470988178
    refval[1] = 1.726962988395339;
    refval[2] = 6.907851953579193;
    refval[3] = 13.815703907167297;
    refval[4] = 0.034949830078124584;   // 0.026983796693676648
    var refvali:Vector.<Number> = new Vector.<Number>(5);
    refvali[0] = 0.524416343749985;     // 0.5245436889646894
    refvali[1] = 2.0974756152524314;
    refvali[2] = 8.389142211032294;
    refvali[3] = 16.778094422092604;
    refvali[4] = 0.03286470898437495;   // 0.03280397441275576
    var dev:Number = Math.abs(JDKtotal - refval[size]);
    var devi:Number = Math.abs(JDKtotali - refvali[size]);
    
    print("Dev: " + dev + " devi: " + devi );
    print("JDK total: " + JDKtotal + " JDKtotali: " + JDKtotali );
    print("\n");
    
    if (dev > 1.0e-12) {
      print("Validation failed");
      print("JDKtotal = " + JDKtotal + "  " + dev + "  "+ size);
    }
    if (devi > 1.0e-12) {
      print("Validation failed");
      print("JDKtotalinverse = " + JDKtotali + "  " + devi + "  " + size);
    }else
    {
      print("Validation sucess");
      print("JDKtotalinverse = " + JDKtotali + "  " + devi + "  " + size);
      return true;
    }
    return false;
  }
    /** Compute Inverse Fast Fourier Transform of (complex) data, in place. */
  function inverse(data:Vector.<Number>):void {
    transform_internal(data, +1);
    // Normalize
    var nd:int = data.length;
    var n:Number = nd / 2;
    var norm:Number = 1/n;
    //print("loop1 in inverse\n");
    for (var i:int = 0; i < nd; i++) {
      data[i] *= norm;
    }
  
    for (var i:int = 0; i < nd; i++) {
      JDKtotali += data[i];
    }
  }
  function RandomVector(N:int, R:int):Vector.<Number> {
    var A = new Vector.<Number>(N);
    for (var i:int = 0; i < N; i++)
    {
      A[i] = _randomDouble()* 1e-6;
    }
    return A;
  }
  /** Compute Fast Fourier Transform of (complex) data, in place. */
  function transform(data:Vector.<Number>):void {
    var JDKrange:int;

    transform_internal(data, -1);

    JDKrange = data.length;
    for (var i:int = 0; i < JDKrange; i++) {
      JDKtotal += data[i];
    }
  }
  
  function transform_internal(data:Vector.<Number>, direction:int) {
    var n:Number = data.length / 2;
    if (n == 1)
    {
      return; // Identity operation!
    }
    var logn:Number = log2(n);

    /* bit reverse the input data for decimation in time algorithm */
    bitreverse(data);

    /* apply fft recursion */
    for (var  bit:int = 0, dual:int = 1; bit < logn; bit++, dual *= 2) {
      var w_real:Number = 1.0;
      var w_imag:Number = 0.0;

      var theta:Number = 2.0 * direction * Math.PI / (2.0 * dual);
      var s:Number = Math.sin(theta);
      var t:Number = Math.sin(theta / 2.0);
      var s2:Number = 2.0 * t * t;
      
      /* a = 0 */
      for (var b:int = 0; b < n; b += 2 * dual) {
        var i:int = 2 * b;
        var j:int = 2 * (b + dual);

        var wd_real:Number = data[j];
        var wd_imag:Number = data[j + 1];

        data[j] = data[i] - wd_real;
        data[j + 1] = data[i + 1] - wd_imag;
        data[i] += wd_real;
        data[i + 1] += wd_imag;
      }

      /* a = 1 .. (dual-1) */
      for (var a:int = 1; a < dual; a++) {
        /* trignometric recurrence for w-> exp(i theta) w */
        {
          var tmp_real:Number = w_real - s * w_imag - s2 * w_real;
          var tmp_imag:Number = w_imag + s * w_real - s2 * w_imag;
          w_real = tmp_real;
          w_imag = tmp_imag;
        }
        for (var b:int = 0; b < n; b += 2 * dual) {
          var i:int = 2 * (b + a);
          var j:int = 2 * (b + a + dual);

          var z1_real:Number = data[j];
          var z1_imag:Number = data[j + 1];

          wd_real = w_real * z1_real - w_imag * z1_imag;
          wd_imag = w_real * z1_imag + w_imag * z1_real;

          data[j] = data[i] - wd_real;
          data[j + 1] = data[i + 1] - wd_imag;
          data[i] += wd_real;
          data[i + 1] += wd_imag;
        }
      }
    }
    //print("leaving transform internal\n");
  }
  
  function bitreverse(data:Vector.<Number>):void {
    /* This is the Goldrader bit-reversal algorithm */
    var n:int = data.length / 2;
    for (var i:int = 0, j:int = 0; i < n - 1; i++) {
      var ii:int = 2 * i;
      var jj:int = 2 * j;
      var k:Number = n / 2;
      if (i < j) {
        var tmp_real:Number = data[ii];
        var tmp_imag:Number = data[ii + 1];
        
        data[ii] = data[jj];
        data[ii + 1] = data[jj + 1];
        data[jj] = tmp_real;
        data[jj + 1] = tmp_imag;
      }

      while (k <= j) {
        j = j - k;
        k = k / 2;
      }
      j += k;
    }
  }
  
  function log2(n:Number):Number {
    var log:Number = 0;
    for (var k:int = 1; k < n; k *= 2, log++)
      ;
    if (n != (1 << log))
    {
      print("FFT: Data length is not a power of 2!: " + n);
      print("log: "+log);
    }
    
    return log;
  } 