function RandomMatrix(M:uint, N:uint, R:Random):Array
  {
        var A:Array = new Array(M);
        for (var i:uint=0; i<N; i++)
                {
                        A[i] = new Array(N);
                        for (var j:uint=0; j<N; j++)
                A[i][j] = R.nextDouble();
                }
                return A;
}
function RandomVector(N:uint, R:Random):Array
        {
                var A:Array = new Array(N);

                for (var i:uint=0; i<N; i++)
                        A[i] = R.nextDouble();
                return A;
}
function NewVectorCopy(x:Array):Array
{
    var N:int = x.length;

    var y:Array = Array(N);
    for (var i:int=0; i<N; i++)
        y[i] = x[i];

    return y;
}

function normabs(x:Array, y:Array):Number
{
    var N:int = x.length;
    var sum:Number = 0.0;

    for (var i:int=0; i<N; i++)
        sum += Math.abs(x[i]-y[i]);

    return sum;
}
function matvec(A:Array, x:Array):Array
        {
                var N = x.length;
                var y:Array = new Array(N);

                matvec3(A, x, y);

                return y;
}
function matvec3(A:Array, x:Array, y:Array):void
{
                var M = A.length;
                var N = A[0].length;

                for (var i:uint=0; i<M; i++)
                {
                        var sum:Number = 0.0;
                        var Ai:Array = A[i];
                        for (var j:uint=0; j<N; j++)
                                sum += Ai[j] * x[j];

                        y[i] = sum;
                }
}
function CopyMatrix(B:Array, A:Array):void
  {
        var M:uint = A.length;
        var N:uint = A[0].length;

                var remainder:uint = N & 3;              // N mod 4;

        for (var i:uint=0; i<M; i++)
        {
            var Bi:Array = B[i];
            var Ai:Array = A[i];
                        for (var j:uint=0; j<remainder; j++)
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
