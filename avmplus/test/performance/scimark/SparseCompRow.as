//package jnt.scimark2;
package {
include "scimarkutil/Constants.as";
include "scimarkutil/Random.as";
include "scimarkutil/Matrix.as";
public class SparseCompRow
{
        /* multiple iterations used to make kernel have roughly
                same granulairty as other Scimark kernels. */

        public static function num_flops(N:uint, nz:uint, num_iterations:uint):Number
        {
                /* Note that if nz does not divide N evenly, then the
                   actual number of nonzeros used is adjusted slightly.
                */
                var actual_nz:uint = (nz/N) * N;
                return (actual_nz) * 2.0 * (num_iterations);
        }


        /* computes  a matrix-vector multiply with a sparse matrix
                held in compress-row format.  If the size of the matrix
                in MxN with nz nonzeros, then the val[] is the nz nonzeros,
                with its ith entry in column col[i].  The integer vector row[]
                is of size M+1 and row[i] points to the begining of the
                ith row in col[].
        */

        public static function matmult( y:Array, val:Array, row:Array,
                col:Array, x:Array, NUM_ITERATIONS:uint):void
        {
                var M:uint = row.length - 1;

                for (var reps:int=0; reps<NUM_ITERATIONS; reps++)
                {
                
                        for (var r:int=0; r<M; r++)
                        {
                                var sum:Number = 0.0;
                                var rowR:int = row[r];
                                var rowRp1:int = row[r+1];
                                for (var i:int=rowR; i<rowRp1; i++)
                                        sum += x[ col[i] ] * val[i];
                                y[r] = sum;
                        }
                }
        }

}
if (CONFIG::desktop) {
    var starttime:Number=new Date();
}
else { // mobile
    var starttime:int=getTimer();
}
var R:Random = new Random(Constants.RANDOM_SEED, 0, 1);
var N:int=Constants.SPARSE_SIZE_M;
var nz:int=Constants.SPARSE_SIZE_nz;

var x:Array = RandomVector(N, R);
var y:Array = new Array(N);

// initialize square sparse matrix
//
// for this test, we create a sparse matrix wit M/nz nonzeros
// per row, with spaced-out evenly between the begining of the
// row to the main diagonal.  Thus, the resulting pattern looks
// like
//             +-----------------+
//             +*                +
//             +***              +
//             +* * *            +
//             +** *  *          +
//             +**  *   *        +
//             +* *   *   *      +
//             +*  *   *    *    +
//             +*   *    *    *  +
//             +-----------------+
//
// (as best reproducible with integer artihmetic)
// Note that the first nr rows will have elements past
// the diagonal.
var nr:int = nz/N;              // average number of nonzeros per row
var anz:int = nr *N;   // _actual_ number of nonzeros
                
var val:Array = RandomVector(anz, R);
var col:Array = Array(anz);
var row:Array = Array(N+1);
var r:uint;
row[0] = 0;
for (r=0; r<N; r++)
{
// initialize elements for row r
var rowr:int = row[r];
row[r+1] = rowr + nr;
var step:int = r/ nr;
if (step < 1) step = 1;   // take at least unit steps


for (var i:uint=0; i<nr; i++)
        col[rowr+i] = i*step;
}

SparseCompRow.matmult(y, val, row, col, x, 400);
if (CONFIG::desktop)
    print("metric time "+(new Date()-starttime));
else // mobile
    print("metric time "+(getTimer()-starttime));
}