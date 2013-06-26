c     end ranfk

c==============================================================================

      subroutine ranfkbinary( K, KBinary )

c------------------------------------------------------------------------------
c     
c     This routine calculates the binary expansion of the argument K, which is a
c     48-bit integer represented as an array of four 12-bit integers.
c     
c     It returns an array of 48 binary values.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer K, KBinary, X, Bits

      dimension K( IRandNumSize ),
     1     KBinary( IBinarySize ),
     2     Bits( Mod4096DigitSize )

c------------------------------------------------------------------------------

      do 300 I = 1, 4
         X = K( I ) / 2
         Bits( 1 ) = iranfodd( K( I ) )

         do 100 J = 2, Mod4096DigitSize 
            Bits( J ) = iranfodd( X )
            X = X / 2
 100     continue

         do 200 J = 1, Mod4096DigitSize
            KBinary( (I-1)*Mod4096DigitSize + J ) = Bits( J )
 200     continue

 300  continue

      return
      end
