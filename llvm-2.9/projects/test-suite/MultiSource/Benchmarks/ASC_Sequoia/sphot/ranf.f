
      double precision function ranf( mySeed )
      include 'params.inc'
      include 'randseed.inc'
      INTEGER mySeed(IRandNumSize)
      real*8  randnum
      CALL pranf(mySeed,randnum)
      ranf = randnum
c      ng_rncount(myStream) = ng_rncount(myStream) + 1
      return
      end
