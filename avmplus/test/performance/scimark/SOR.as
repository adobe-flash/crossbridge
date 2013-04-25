package {
include "scimarkutil/Constants.as";
include "scimarkutil/Random.as";
include "scimarkutil/Matrix.as";
class SOR
{
        public static function num_flops(M:uint, N:uint, num_iterations:uint) : Number
        {
                var Md : Number =  Number(M);
                var Nd : Number =  Number(N);
                var num_iterD : Number  = Number(num_iterations);

                return (Md-1)*(Nd-1)*num_iterD*6.0;
        }

        public static function execute( omega:Number , G:Array,
                        num_iterations:uint)
        {
                var M : Number = G.length;
                var N : Number = G[0].length;

                var omega_over_four : Number = omega * 0.25;
                var one_minus_omega : Number = 1.0 - omega;

                // update interior points
                //
                var Mm1 : Number = M-1; // var Mm1 : uint = M-1;
                var Nm1 : Number = N-1;  //var Nm1 : uint = N-1;
                for (var p:Number=0; p<num_iterations; p++)
                {
                        for (var i:Number=1; i<Mm1; i++)
                        {
                                var Gi:Array  = G[i]; // double[] Gi = G[i];
                                var Gim1:Array  = G[i-1];
                                var Gip1:Array = G[i+1];
                                for (var j:Number=1; j<Nm1; j++)
                                {
                                        Gi[j] = omega_over_four * (Gim1[j] + Gip1[j] + Gi[j-1]
                                                                + Gi[j+1]) + one_minus_omega * Gi[j];
                                }
                        }
                }
        }
}
if (CONFIG::desktop) {
    var starttime:Number=new Date();
    var cycles:int=1000;
}
else { // mobile
    var starttime:int=getTimer();
    var cycles:int=100;
}
var R:Random = new Random(Constants.RANDOM_SEED, 0, 1);
var N:Number = Constants.SOR_SIZE;
var G:Array = RandomMatrix(N, N, R);

SOR.execute(1.25, G, cycles);
if (CONFIG::desktop)
    print("metric time "+(new Date()-starttime));
else // mobile
    print("metric time "+(getTimer()-starttime));
}