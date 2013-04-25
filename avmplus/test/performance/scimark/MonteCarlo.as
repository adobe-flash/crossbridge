//package jnt.scimark2;
package {
include "scimarkutil/Random.as";
/**
 Estimate Pi by approximating the area of a circle.

 How: generate N random numbers in the unit square, (0,0) to (1,1)
 and see how are within a radius of 1 or less, i.e.
 <pre>

 sqrt(x^2 + y^2) < r

 </pre>
  since the radius is 1.0, we can square both sides
  and avoid a sqrt() computation:
  <pre>

    x^2 + y^2 <= 1.0

  </pre>
  this area under the curve is (Pi * r^2)/ 4.0,
  and the area of the unit of square is 1.0,
  so Pi can be approximated by
  <pre>
                        # points with x^2+y^2 < 1
     Pi =~              --------------------------  * 4.0
                             total # points

  </pre>

*/

public class MonteCarlo
{
         static var SEED:uint = 113;

        public static  function num_flops(Num_samples:uint):Number
        {
                // 3 flops in x^2+y^2 and 1 flop in random routine

                return (Num_samples)* 4.0;

        }

        

        public static  function integrate(Num_samples:uint):Number
        {

                var R:Random = new Random(SEED, 0, 1);

                var under_curve:uint = 0;
                for (var count:Number=0; count<Num_samples; count++)
                {
                        var x:Number = R.nextDouble();
                        var y:Number = R.nextDouble();

                        if ( x*x + y*y <= 1.0)
                                 under_curve ++;
                        
                }

                return (under_curve / Num_samples) * 4.0;
        }


}
if (CONFIG::desktop) {
    var starttime:Number=new Date();
    var cycles:uint=15000000;
}
else { // mobile
    var starttime:int=getTimer();
    var cycles:uint=15000;
}

MonteCarlo.integrate(cycles);
if (CONFIG::desktop)
    print("metric time "+(new Date()-starttime));
else // mobile
    print("metric time "+(getTimer()-starttime));

}