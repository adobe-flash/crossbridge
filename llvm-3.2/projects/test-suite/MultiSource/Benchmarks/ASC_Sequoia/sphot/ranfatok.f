c     end Rans

c==============================================================================

      subroutine ranfatok( a, Kbinary, atothek )

c------------------------------------------------------------------------------
c     
c     This routine calculates a to the Kth power, mod 2**48. K is a binary number.
c     
c     It returns the calculated value as an array of four modulo-4096 digits.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer a, KBinary, atothek, asubi

      dimension a( IRandNumSize ),
     1     KBinary( IBinarySize ),
     2     atothek( IRandNumSize ),
     3     asubi( IRandNumSize )

c...MJC mod...this was missing
      INTEGER IZero
      PARAMETER( IZero = 0 )

c------------------------------------------------------------------------------

c**** The following amounts to the first iteration of a 46-loop.
      asubi( 1 ) = a( 1 )
      asubi( 2 ) = a( 2 )
      asubi( 3 ) = a( 3 )
      asubi( 4 ) = a( 4 )

      atothek( 1 ) = 1
      atothek( 2 ) = IZero
      atothek( 3 ) = IZero
      atothek( 4 ) = IZero

      do 100 I = 1, 45
         if( KBinary( I ) .ne. IZero ) then
            CALL ranfmodmult( atothek, asubi, atothek )
         endif
         CALL ranfmodmult( asubi, asubi, asubi )
 100  continue

      return
      end
