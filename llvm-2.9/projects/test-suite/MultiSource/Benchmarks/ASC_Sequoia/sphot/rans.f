c     end PRanf

c==============================================================================

      subroutine Rans( NIn, Seed1, SeedArray )

c------------------------------------------------------------------------------
c     
c     This routine divides the sequence of random numbers into N subsequences,
c     each with its own seed.  The seeds for the independent subsequences are
c     returned in the seed array.  if Seed1 is zero, all zeroes will be returned.
c     To prevent this, Seed1 is set to [3281, 4041, 595, 2376], which is 
c     statistically the best starting seed.  The wheel is then divided into the
c     N pieces (where N is odd and >= NIn) by dividing its period (2**46) by N.
c     
c     Then, seed(i) = seed(i-1) * (a**k mod 2**48), and 1<=k<=N.
c     
c     Here, 'a' is the multiplier used by the linear congruential generator whose
c     wheel we are dividing up.
c     
c     The number of streams must be odd; if NIn is even N will be NIn+1, and
c     n extra stream of random numbers will be available that will not get used.
c     
c     It returns an array of seeds, each an array of 4 integers that are used as
c     the digits of a four-digit modulo-4096 integer.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer NIn, Seed1, DefaultValues, SeedArray, N, atothek, K, 
     1     KBinary, InSeed, OutSeed

c******************************************************************************
c**** Data common to the PRanf package.
      integer Multiplier, DefaultSeed

      real*8 Divisor

      dimension Multiplier( IRandNumSize ), DefaultSeed( IRandNumSize ),
     1     Divisor( IRandNumSize )

      common / PRanfCom / Multiplier, DefaultSeed, Divisor 
c**** End of PRanf common data
c******************************************************************************

      dimension DefaultValues( IRandNumSize ),
     1     SeedArray( IRandNumSize, MaxStreams ),
     2     atothek( IRandNumSize ),
     3     K( IRandNumSize ),
     4     KBinary( IBinarySize ),
     5     InSeed( IRandNumSize ),
     6     OutSeed( IRandNumSize )

      INTEGER NisOdd

c     scatter, shared SeedArray

c------------------------------------------------------------------------------


c     write( *, 10 ) NIn, Seed1
 10   format( ' Rans: NIn = ', i6, '  Seed1 = ', i6, /,
     1     '------' )

      IZero = 0

c**** Make sure we are generating an odd number of random number sequences.
      if( NisOdd( NIn ) .eq. 1 ) then
         N = NIN
      else
         N = NIn + 1
      endif

c**** Set up the initial seed to either a legal input value or its default
c**** values. The input integer, if nonzero, is used for the first of the
c**** four modulo-4096 digits of the actual initial seed.

c**** The First element of the returned SeedArray will be used here, since
c**** at least one seed will be returned, and the first seed of the set 
c**** returned will be the seed for the entire wheel.
      if( Seed1 .eq. IZero ) then
         SeedArray( 1, 1 ) = DefaultSeed( 1 )
         SeedArray( 2, 1 ) = DefaultSeed( 2 )
         SeedArray( 3, 1 ) = DefaultSeed( 3 )
         SeedArray( 4, 1 ) = DefaultSeed( 4 )
      else
         SeedArray( 1, 1 ) = abs( Seed1 )
         SeedArray( 2, 1 ) = IZero
         SeedArray( 3, 1 ) = IZero
         SeedArray( 4, 1 ) = IZero
      endif

c**** 'a' is the multiplier for the Ranf linear congruential generator.
      if( N .eq. 1 ) then
c*******If only one stream of random numbers is needed, do not bother to 
c*******raise 'a' to the first power.
         atothek( 1 ) = Multiplier( 1 )
         atothek( 2 ) = Multiplier( 2 )
         atothek( 3 ) = Multiplier( 3 )
         atothek( 4 ) = Multiplier( 4 )
      else
c*******more than one stream is needed; generate the Kth seed by multiplying
c*******the K-1st seed by the multiplier raised to the Nth power.
         CALL ranfk( N, K )
         CALL ranfkbinary( K, KBinary )
         CALL ranfatok( Multiplier, KBinary, atothek )
         do 100 I = 2, N
            InSeed( 1 ) = SeedArray( 1, I-1 )
            InSeed( 2 ) = SeedArray( 2, I-1 )
            InSeed( 3 ) = SeedArray( 3, I-1 )
            InSeed( 4 ) = SeedArray( 4, I-1 )
            CALL ranfmodmult( InSeed, atothek, OutSeed )
            SeedArray( 1, I ) = OutSeed( 1 )
            SeedArray( 2, I ) = OutSeed( 2 )
            SeedArray( 3, I ) = OutSeed( 3 )
            SeedArray( 4, I ) = OutSeed( 4 )
 100     continue
      endif

c     write( *, 110 )
 110  format( '   leaving Rans' )

      return
      end


c MJC mod to remove statement function
      INTEGER FUNCTION NISODD( M )
c****  check for argument numbers being odd.
      NIsOdd = mod( m, 2 )
      RETURN
      END
