
/* Random.java based on Java Numerical Toolkit (JNT) Random.UniformSequence
        class.  We do not use Java's own java.util.Random so that we can compare
        results with equivalent C and Fortran coces.
*/

public class Random {


/* ------------------------------------------------------------------------------
                               CLASS VARIABLES
   ------------------------------------------------------------------------------ */

  var seed : Number = 0;

  private var m : Array;
  private var i : uint = 4;
  private var j : uint = 16;

  private var mdig : uint = 32;
  private var one : uint = 1;
  private var m1 : uint = (one << mdig-2) + ((one << mdig-2)-one);
  private var m2 : uint = one << mdig/2;

  /* For mdig = 32 : m1 =          2147483647, m2 =      65536
     For mdig = 64 : m1 = 9223372036854775807, m2 = 4294967296
  */

  private var dm1 : Number = 1.0 / m1;

  private var haveRange : Boolean = false;
  private var left : Number  = 0.0;
  private var right : Number = 1.0;
  private var width : Number = 1.0;


/* ------------------------------------------------------------------------------
                                CONSTRUCTORS
   ------------------------------------------------------------------------------ */

/**
   Initializes a sequence of uniformly distributed quasi random numbers with a
   seed based on the system clock.
*/

/*
  public function Random () {
    initialize( new Date().getTime() )
  }
*/
/**
   Initializes a sequence of uniformly distributed quasi random numbers on a
   given half-open interval [left,right) with a seed based on the system
   clock.

@param <B>left</B> (double)<BR>

       The left endpoint of the half-open interval [left,right).

@param <B>right</B> (double)<BR>

       The right endpoint of the half-open interval [left,right).
*/

/*
  public function Random (  left : Number,  right : Number) {
    initialize( new Date().getTime() );
    this.left = left;
    this.right = right;
    width = right - left;
    haveRange = true;
  }
*/
/**
   Initializes a sequence of uniformly distributed quasi random numbers with a
   given seed.

@param <B>seed</B> (int)<BR>

       The seed of the random number generator.  Two sequences with the same
       seed will be identical.
*/

// function overrides not allowed in as
/*
  public function Random ( seed : Number) {
    initialize( seed);
  }
  */

/**
   Initializes a sequence of uniformly distributed quasi random numbers
   with a given seed on a given half-open interval [left,right).

@param <B>seed</B> (int)<BR>

       The seed of the random number generator.  Two sequences with the same
       seed will be identical.

@param <B>left</B> (double)<BR>

       The left endpoint of the half-open interval [left,right).

@param <B>right</B> (double)<BR>

       The right endpoint of the half-open interval [left,right).
*/
  public function Random ( seed : Number, left : Number, right : Number) {
    initialize( seed);
    this.left = left;
    this.right = right;
    width = right - left;
    haveRange = true;
  }

/* ------------------------------------------------------------------------------
                             PUBLIC METHODS
   ------------------------------------------------------------------------------ */

/**
   Returns the next random number in the sequence.
*/
  public final function nextDouble () : Number {

    var k:Number;
    var nextValue:Number;

    k = m[i] - m[j];
    if (k < 0) k += m1;
    m[j] = k;

    if (i == 0)
                i = 16;
        else i--;

    if (j == 0)
                j = 16 ;
        else j--;

    if (haveRange)
                return  left +  dm1 * k * width;
        else
                return dm1 * k;

  }

/**
   Returns the next N random numbers in the sequence, as
   a vector.
*/
  public final function nextDoubles (x : Array) : void
  {

        var N:uint = x.length;
        var remainder:uint = N & 3;             // N mod 4
        var k:uint;
        var count:uint;

        if (haveRange)
        {
                for (count=0; count<N; count++)
                {
                k = m[i] - m[j];

                if (i == 0) i = 16;
                                else i--;
                                
                if (k < 0) k += m1;
                m[j] = k;

                if (j == 0) j = 16;
                                else j--;

                x[count] = left + dm1 * k * width;
                }
        
        }
        else
        {

                for (count=0; count<remainder; count++)
                {
                k = m[i] - m[j];

                if (i == 0) i = 16;
                                else i--;

                if (k < 0) k += m1;
                m[j] = k;

                if (j == 0) j = 16;
                                else j--;


                x[count] = dm1 * k;
                }

                for (count=remainder; count<N; count+=4)
                {
                k = m[i] - m[j];
                if (i == 0) i = 16;
                                else i--;
                if (k < 0) k += m1;
                m[j] = k;
                if (j == 0) j = 16;
                                else j--;
                x[count] = dm1 * k;


                k = m[i] - m[j];
                if (i == 0) i = 16;
                                else i--;
                if (k < 0) k += m1;
                m[j] = k;
                if (j == 0) j = 16;
                                else j--;
                x[count+1] = dm1 * k;


                k = m[i] - m[j];
                if (i == 0) i = 16;
                                else i--;
                if (k < 0) k += m1;
                m[j] = k;
                if (j == 0) j = 16;
                                else j--;
                x[count+2] = dm1 * k;


                k = m[i] - m[j];
                if (i == 0) i = 16;
                                else i--;
                if (k < 0) k += m1;
                m[j] = k;
                if (j == 0) j = 16;
                                else j--;
                x[count+3] = dm1 * k;
                }
        }
  }

                
   



/*----------------------------------------------------------------------------
                           PRIVATE METHODS
  ------------------------------------------------------------------------ */

  private function initialize (seed:Number) : void {

    var jseed, k0, k1, j0, j1, iloop;

    this.seed = seed;

    m = new Array(17); //[17];

    jseed = Math.min(Math.abs(seed),m1);
    if (jseed % 2 == 0) --jseed;
    k0 = 9069 % m2;
    k1 = 9069 / m2;
    j0 = jseed % m2;
    j1 = jseed / m2;
    for (iloop = 0; iloop < 17; ++iloop)
        {
                jseed = j0 * k0;
                j1 = (jseed / m2 + j0 * k1 + j1 * k0) % (m2 / 2);
                j0 = jseed % m2;
                m[iloop] = j0 + m2 * j1;
    }
    i = 4;
    j = 16;

  }

}
