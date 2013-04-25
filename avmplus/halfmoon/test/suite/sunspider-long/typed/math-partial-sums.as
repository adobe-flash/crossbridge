package {
  
  // dupe of above function used to verify that test is running correctly
  function verifyTest():Boolean{
      var n:int = 2000;
      var a1:Number,a2:Number,a3:Number,a4:Number,a5:Number,a6:Number,a7:Number,a8:Number,a9:Number;
      a1 = a2 = a3 = a4 = a5 = a6 = a7 = a8 = a9 = 0.0;
      var twothirds:Number = 2.0/3.0;
      var alt:Number = -1.0;
	 var k3:Number = 0, sk = 0, ck = 0;
      var k2:Number = k3; 
      
      for (var k:int = 1; k <= n; k++){
          k2 = k*k;
          k3 = k2*k;
          sk = Math.sin(k);
          ck = Math.cos(k);
          alt = -alt;
          
          a1 += Math.pow(twothirds,k-1);
          a2 += Math.pow(k,-0.5);
          a3 += 1.0/(k*(k+1.0));
          a4 += 1.0/(k3 * sk*sk);
          a5 += 1.0/(k3 * ck*ck);
          a6 += 1.0/k;
          a7 += 1.0/k2;
          a8 += alt/k;
          a9 += alt/(2*k -1);
      }
      var a:Array = [a2,a2,a3,a4,a5,a6,a7,a8,a9];
      var expectedAns:Array = [87.9935444652217,87.9935444652217,0.9995002498750635,30.307963126682765,42.99485321081218,8.178368103610284,1.6444341918273961,0.6928972430599403,0.7852731634052603];
      for (var i=0; i<a.length; i++) {
        if (Math.abs(a[i]-expectedAns[i]) > .00001) {
          print('a'+i+': '+a[i]+' !== expectedAns: '+expectedAns[i]+" "+(a[i]-expectedAns[i]));
          return false;
        }
      }
      return true;
  }

	for (var i:int = 0; i < 30000; i++) {
		verifyTest();
	}

}
