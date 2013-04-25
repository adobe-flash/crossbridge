package {
include "scimarkutil/Constants.as";
include "scimarkutil/Random.as";
include "scimarkutil/Matrix.as";
//include "scimark/kernel.as";
//package jnt.scimark2;

/** Computes FFT's of complex, double precision data where n is an integer power of 2.
  * This appears to be slower than the Radix2 method,
  * but the code is smaller and simpler, and it requires no extra storage.
  * <P>
  *
  * @author Bruce R. Miller bruce.miller@nist.gov,
  * @author Derived from GSL (Gnu Scientific Library),
  * @author GSL's FFT Code by Brian Gough bjg@vvv.lanl.gov
  */

  /* See {@link ComplexDoubleFFT ComplexDoubleFFT} for details of data layout.
   */

public class FFT {

  public static  function num_flops(N:uint):Number
  {
         var Nd:Number = N;
         var logN:Number = log2(N);

         return (5.0*Nd-2)*logN + 2*(Nd+1);
   }


  /** Compute Fast Fourier Transform of (complex) data, in place.*/
  public static function  transform (data:Array):void {
    transform_internal(data, -1); }

  /** Compute Inverse Fast Fourier Transform of (complex) data, in place.*/
  public static function inverse (data:Array):void {
    transform_internal(data, +1);
    // Normalize
    var nd:uint=data.length;
    var n:uint =nd/2;
    var norm:Number=1/(n);
    for(var i:Number=0; i<nd; i++)
      data[i] *= norm;
  }

  /** Accuracy check on FFT of data. Make a copy of data, Compute the FFT, then
    * the inverse and compare to the original.  Returns the rms difference.*/
  public static function test(data:Array):Number {
    var nd:uint = data.length;
    // Make duplicate for comparison
    var copy:Array = new Array(nd);
        for(var i:Number=0; i<nd;i++)
                copy[i] = data[i];

    // Transform & invert
    transform(data);
    inverse(data);
    // Compute RMS difference.
    var  diff:Number = 0.0;
    for(i=0; i<nd; i++) {
      var d:Number = data[i]-copy[i];
      diff += d*d; }
    return Math.sqrt(diff/nd); }

  /** Make a random array of n (complex) elements. */
  public static function makeRandom(n:uint):Array {
    var nd:uint = 2*n;
    var data:Array = new Array(nd);
    for(var i:Number=0; i<nd; i++)
      data[i]= Math.random();
    return data; }

  /* ______________________________________________________________________ */

   static function log2 (n:uint):uint {
    var log:uint = 0;
    for(var k:Number=1; k < n; k *= 2, log++);
    if (n != (1 << log))
      throw new Error("FFT: Data length is not a power of 2!: "+n);
    return log; }

   static function transform_internal (data:Array, direction:Number):void {
        if (data.length == 0) return;
        var n:uint = data.length/2;
    if (n == 1) return;         // Identity operation!
    var logn:uint = log2(n);

    /* bit reverse the input data for decimation in time algorithm */
    bitreverse(data) ;

    /* apply fft recursion */
        /* this loop executed log2(N) times */
    for (var bit:Number = 0, dual = 1; bit < logn; bit++, dual *= 2) {
      var w_real:Number = 1.0;
      var w_imag:Number = 0.0;

      var theta:Number = 2.0 * direction * Math.PI / (2.0 * dual);
      var s:Number = Math.sin(theta);
      var t:Number = Math.sin(theta / 2.0);
      var s2:Number = 2.0 * t * t;

      /* a = 0 */
      for (var b:Number = 0; b < n; b += 2 * dual) {
        var i:uint = 2*b ;
        var j:uint = 2*(b + dual);

        var wd_real:Number = data[j] ;
        var wd_imag:Number = data[j+1] ;
          
        data[j]   = data[i]   - wd_real;
        data[j+1] = data[i+1] - wd_imag;
        data[i]  += wd_real;
        data[i+1]+= wd_imag;
      }
      
      /* a = 1 .. (dual-1) */
      for (var a:uint = 1; a < dual; a++) {
        /* trignometric recurrence for w-> exp(i theta) w */
        {
          var tmp_real:Number = w_real - s * w_imag - s2 * w_real;
          var tmp_imag:Number = w_imag + s * w_real - s2 * w_imag;
          w_real = tmp_real;
          w_imag = tmp_imag;
        }
        for (b = 0; b < n; b += 2 * dual) {
          i = 2*(b + a);
          j = 2*(b + a + dual);

          var z1_real:Number = data[j];
          var z1_imag:Number = data[j+1];
              
          wd_real = w_real * z1_real - w_imag * z1_imag;
          wd_imag = w_real * z1_imag + w_imag * z1_real;

          data[j]   = data[i]   - wd_real;
          data[j+1] = data[i+1] - wd_imag;
          data[i]  += wd_real;
          data[i+1]+= wd_imag;
        }
      }
    }
  }


   static function bitreverse(data:Array):void {
    /* This is the Goldrader bit-reversal algorithm */
    var n:uint = data.length / 2;
        var nm1:uint = n-1;
        var i:Number=0;
        var j:Number=0;
    for (; i < nm1; i++) {

      //int ii = 2*i;
      var ii:Number = i << 1;

      //int jj = 2*j;
      var jj:Number = j << 1;

      //int k = n / 2 ;
      var k:Number = n >> 1;

      if (i < j) {
        var tmp_real:Number    = data[ii];
        var tmp_imag:Number    = data[ii+1];
        data[ii]   = data[jj];
        data[ii+1] = data[jj+1];
        data[jj]   = tmp_real;
        data[jj+1] = tmp_imag; }

      while (k <= j)
          {
        //j = j - k ;
                j -= k;

        //k = k / 2 ;
        k >>= 1 ;
          }
      j += k ;
    }
  }
}
// main
if (CONFIG::desktop) {
    var starttime:Number=new Date();
    var cycles:int = 600;
}
else { // mobile
    var starttime:int=getTimer();
    var cycles:int = 50;
}

var R:Random = new Random(Constants.RANDOM_SEED, 0, 1);
var N:int = Constants.FFT_SIZE;
var x:Array = RandomVector(2*N, R);
for (var i:uint=0; i<cycles; i++){
        FFT.transform(x); // forward transform
        FFT.inverse(x); // backward transform
}



if (CONFIG::desktop)
    var endtime:Number = new Date()-starttime;
else // mobile
    var endtime:int = getTimer()-starttime;


// Validate that the code ran correctly
const EPS:Number = 1.0e-10;
if ( FFT.test(x) / N > EPS )
    print("FAILED verification.");
else
    print("metric time "+endtime);


}