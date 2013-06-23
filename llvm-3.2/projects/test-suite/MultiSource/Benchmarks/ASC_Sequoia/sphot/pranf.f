c==============================================================================
c     
c     R A N D O M   N U M B E R   G E N E R A T O R
c     
c==============================================================================

      subroutine pranf( Seed, RandNum )

c------------------------------------------------------------------------------
c     
c     This is the one of the two user-callable routines of a linear congruential
c     random number generator, modeled after the RANF generator for the Cray.
c     This routine generates the next random number in the sequence and a new seed
c     for the remainder of the sequence.  The seed and the random number are the
c     same, but are returned in different form: the random number is a fortran
c     'real', but the seed is an array of four words, each containing an integer
c     that is used internally to the generator as one digit of a four-digit,
c     modulo-4096 integer.
c     
c     It returns the new random number and a new seed.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer Seed( IRandNumSize )

      real*8 RandNum

c******************************************************************************
c**** Data common to the PRanf package.
      integer Multiplier( IRandNumSize ), DefaultSeed( IRandNumSize )
      real*8 Divisor( IRandNumSize )

      common / PRanfCom / Multiplier, DefaultSeed, Divisor 


c**** End of PRanf common data
c******************************************************************************

c------------------------------------------------------------------------------

c     write( *, 10 )
 10   format( ' Ranf', /, '-----' )

      RandNum = float( Seed( 4 ) ) / Divisor( 4 ) +
     1     float( Seed( 3 ) ) / Divisor( 3 ) +
     2     float( Seed( 2 ) ) / Divisor( 2 ) +
     3     float( Seed( 1 ) ) / Divisor( 1 ) 

      CALL ranfmodmult( Multiplier, Seed, Seed )

c     write( *, 20 )
 20   format( '  leaving Ranf' )

      return
      end

      BLOCK DATA PINIT

      include 'pranf.inc'
      integer Multiplier( IRandNumSize ), DefaultSeed( IRandNumSize )
      real*8 Divisor( IRandNumSize )

      common / PRanfCom / Multiplier, DefaultSeed, Divisor 

      data Multiplier / 373, 3707, 1442, 647 /
      data DefaultSeed / 3281, 4041, 595, 2376 / 
      data Divisor / 281474976710656.0, 68719476736.0, 16777216.0, 
     &                  4096.0 /

      END
