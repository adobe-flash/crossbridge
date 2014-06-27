
      subroutine copyseed( myIter, myStream, mySeed, firstRanf )

      include 'params.inc'
      include 'randseed.inc'
      include 'shared.inc'

      integer myIter, mystream, myseed(*)
      REAL*8 firstRanf
      REAL*8 fran, ranf
      external ranf

c     
c     dale's mod
c     
      if (myStream .gt. 0) then
         do 2 i = 1 ,IrandNumSize
            seedarray(i, myStream) = mySeed(i)
 2       continue     
      endif
      myStream = myIter 
c     
c     ng_rncount(myStream) = 0
      do 1 i = 1, IrandNumSize
         mySeed(i) = seedarray(i,myStream)
    1 continue


      firstRanf = ranf(mySeed)

      return
      end
