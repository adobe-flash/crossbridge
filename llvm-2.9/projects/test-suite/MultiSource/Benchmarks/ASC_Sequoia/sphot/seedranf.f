

      subroutine seedranf( numStreams )
      include 'randseed.inc'
      integer*4 numStreams
      integer*4 tempNumStreams  !Because rans may change numStreams.
      NStreams = numStreams
      tempNumStreams = numStreams
      CALL rans(tempNumStreams,0,seedarray)
      return 
      end
