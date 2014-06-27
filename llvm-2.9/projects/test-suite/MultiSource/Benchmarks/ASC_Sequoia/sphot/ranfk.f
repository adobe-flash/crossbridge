c     end iranfeven

c==============================================================================

      subroutine ranfk( N, K )

c------------------------------------------------------------------------------
c     
c     This routine calculates 2**46/N, which should be the period of each of the
c     subsequences of random numbers that are being created. Both the numerator
c     and the result of this calculation are represented as an array of four
c     integers, each of which is one digit of a four-digit moduo-4096 number.  The
c     numerator is represented as (1024, 0, 0, 0 ), using base ten digits.
c     
c     It returns the result of the division.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer N, K, nn, r4, r3, r2, r1, q4, q3, q2, q1

      dimension K( IRandNumSize )

c------------------------------------------------------------------------------

      nn = N + iranfeven( N )

      q4 = 1024 / nn
      r4 = 1024 - (nn * q4)
      q3 = (r4 * 4096) / nn
      r3 = (r4 * 4096) - (nn * q3)
      q2 = (r3 * 4096) / nn
      r2 = (r3 * 4096) - (nn * q2)
      q1 = (r2 * 4096) / nn

      K( 1 ) = q1
      K( 2 ) = q2
      K( 3 ) = q3
      K( 4 ) = q4

      return
      end
