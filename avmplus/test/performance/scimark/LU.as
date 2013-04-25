//package jnt.scimark2;
package {
include "scimarkutil/Constants.as";
include "scimarkutil/Random.as";
include "scimarkutil/Matrix.as";
/**
    LU matrix factorization. (Based on TNT implementation.)
    Decomposes a matrix A  into a triangular lower triangular
    factor (L) and an upper triangular factor (U) such that
    A = L*U.  By convnetion, the main diagonal of L consists
    of 1's so that L and U can be stored compactly in
    a NxN matrix.


*/
public class LU
{
    /**
        Returns a <em>copy</em> of the compact LU factorization.
        (useful mainly for debugging.)

        @return the compact LU factorization.  The U factor
        is stored in the upper triangular portion, and the L
        factor is stored in the lower triangular portion.
        The main diagonal of L consists (by convention) of
        ones, and is not explicitly stored.
    */


        public static  function num_flops(N:uint):Number
        {
                // rougly 2/3*N^3

                var Nd:Number =  N;

                return (2.0 * Nd *Nd *Nd/ 3.0);
        }

     static function new_copyN( x:Array):Array
    {
        var N = x.length;
        var T:Array = new Array(N);
        for (var i=0; i<N; i++)
            T[i] = x[i];
        return T;
    }


     static function new_copyMN( A:Array):Array
    {
        var M = A.length;
        var N = A[0].length;

        var T:Array = new Array(M);
                for(var i:Number=0; i<M; i++)
                        T[i] = new Array(N);

        for (i=0; i<M; i++)
        {
            var Ti:Array = T[i];
            var Ai:Array = A[i];
            for (var j:Number=0; j<N; j++)
                Ti[j] = Ai[j];
        }

        return T;
    }



    public static function new_copyNInt( x:Array):Array
    {
        var N = x.length;
        var T:Array = new Array(N);
        for (var i:Number=0; i<N; i++)
            T[i] = x[i];
        return T;
    }

    static  function insert_copy(B:Array, A:Array):void
    {
        var M:uint = A.length;
        var N:uint = A[0].length;

                var remainder = N & 3;           // N mod 4;

        for (var i=0; i<M; i++)
        {
            var Bi:Array = B[i];
            var Ai:Array = A[i];
                        for (var j:Number=0; j<remainder; j++)
                Bi[j] = Ai[j];
            for (j=remainder; j<N; j+=4)
                        {
                                Bi[j] = Ai[j];
                                Bi[j+1] = Ai[j+1];
                                Bi[j+2] = Ai[j+2];
                                Bi[j+3] = Ai[j+3];
                        }
                }
        
    }
    public function getLU():Array
    {
        return new_copyMN(LU_);
    }

    /**
        Returns a <em>copy</em> of the pivot vector.

        @return the pivot vector used in obtaining the
        LU factorzation.  Subsequent solutions must
        permute the right-hand side by this vector.

    */
    public function getPivot():Array
    {
        return new_copyNInt(pivot_);
    }
    
    /**
        Initalize LU factorization from matrix.

        @param A (in) the matrix to associate with this
                factorization.
    */
    public function LU( A:Array )
    {
        var M:uint = A.length;
        var N:uint = A[0].length;

        //if ( LU_ == null || LU_.length != M || LU_[0].length != N)
        LU_ = new Array(M);
                for(var i:Number=0; i<M; i++)
                        LU_[i] = new Array(N);


        insert_copy(LU_, A);

        //if (pivot_.length != M)
        pivot_ = new Array(M);

        factor(LU_, pivot_);
    }

    /**
        Solve a linear system, with pre-computed factorization.

        @param b (in) the right-hand side.
        @return solution vector.
    */
    public function solve1(b:Array):Array
    {
        var x:Array = new_copyN(b);

        solve(LU_, pivot_, x);
        return x;
    }
    

/**
    LU factorization (in place).

    @param A (in/out) On input, the matrix to be factored.
        On output, the compact LU factorization.

    @param pivit (out) The pivot vector records the
        reordering of the rows of A during factorization.
        
    @return 0, if OK, nozero value, othewise.
*/
public static function factor(A:Array,  pivot:Array):uint
{
 


    var N:uint = A.length;
    var M:uint = A[0].length;

    var minMN:uint = Math.min(M,N);

    for (var j:Number=0; j<minMN; j++)
    {
        // find pivot in column j and  test for singularity.

        var jp:uint=j;
        
        var t:Number = Math.abs(A[j][j]);
        for (var i:Number=j+1; i<M; i++)
        {
            var ab:Number = Math.abs(A[i][j]);
            if ( ab > t)
            {
                jp = i;
                t = ab;
            }
        }
        
        pivot[j] = jp;

        // jp now has the index of maximum element
        // of column j, below the diagonal

        if ( A[jp][j] == 0 )
            return 1;       // factorization failed because of zero pivot


        if (jp != j)
        {
            // swap rows j and jp
            var tA:Array = A[j];
            A[j] = A[jp];
            A[jp] = tA;
        }

        if (j<M-1)                // compute elements j+1:M of jth column
        {
            // note A(j,j), was A(jp,p) previously which was
            // guarranteed not to be zero (Label #1)
            //
            var recp:Number =  1.0 / A[j][j];

            for (var k:Number=j+1; k<M; k++)
                A[k][j] *= recp;
        }


        if (j < minMN-1)
        {
            // rank-1 update to trailing submatrix:   E = E - x*y;
            //
            // E is the region A(j+1:M, j+1:N)
            // x is the column vector A(j+1:M,j)
            // y is row vector A(j,j+1:N)


            for (var ii:Number=j+1; ii<M; ii++)
            {
                var Aii:Array = A[ii];
                var Aj:Array = A[j];
                var AiiJ:Number = Aii[j];
                for (var jj=j+1; jj<N; jj++)
                  Aii[jj] -= AiiJ * Aj[jj];

            }
        }
    }

    return 0;
}


    /**
        Solve a linear system, using a prefactored matrix
            in LU form.


        @param LU (in) the factored matrix in LU form.
        @param pivot (in) the pivot vector which lists
            the reordering used during the factorization
            stage.
        @param b    (in/out) On input, the right-hand side.
                    On output, the solution vector.
    */
    public static function solve(LU:Array, pvt:Array, b:Array):void
    {
        var M:uint = LU.length;
        var N:uint = LU[0].length;
        var ii:uint=0;

        for (var i:Number=0; i<M; i++)
        {
            var ip:uint = pvt[i];
            var sum:Number = b[ip];

            b[ip] = b[i];
            if (ii==0)
                for (var j:uint=ii; j<i; j++)
                    sum -= LU[i][j] * b[j];
            else
                if (sum == 0.0)
                    ii = i;
            b[i] = sum;
        }

        for ( i=N-1; i>=0; i--)
        {
            sum = b[i];
            for ( j=i+1; j<N; j++)
                sum -= LU[i][j] * b[j];
            b[i] = sum / LU[i][i];
        }
    }


    private var LU_:Array;
    private var pivot_:Array;
}
if (CONFIG::desktop) {
    var starttime:Number=new Date();
    var cycles:Number=80;
}
else { // mobile
    var starttime:int=getTimer();
    var cycles:Number=10;
}
var R:Random = new Random(Constants.RANDOM_SEED, 0, 1);
var N:Number=Constants.LU_SIZE;
var A:Array = RandomMatrix(N, N,  R);
var lu:Array = RandomMatrix(N, N, R);  // doesn't need random but
var pivot:Array = new Array(N);
for (var i:int=0;i<cycles;i++) {
  CopyMatrix(lu, A);
  LU.factor(lu, pivot);
}

if (CONFIG::desktop)
    var endtime:Number = new Date()-starttime;
else // mobile
    var endtime:int = getTimer()-starttime;


// verify that LU is correct
var b:Array = RandomVector(N, R);
var x:Array = NewVectorCopy(b);

LU.solve(lu, pivot, x);

const EPS:Number = 1.0e-12;
if ( normabs(b, matvec(A,x)) / N > EPS )
    print("FAILED verification.");
else
    print("metric time "+endtime);




}