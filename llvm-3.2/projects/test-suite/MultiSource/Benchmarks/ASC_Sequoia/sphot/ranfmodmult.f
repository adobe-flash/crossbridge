c     end ranfkbinary

c==============================================================================

      subroutine ranfmodmult( A, B, C )

c------------------------------------------------------------------------------
c     
c     Ths routine calculates the product of the first two arguments.  All three
c     arguments are represented as arrays of 12-bit integers, each making up
c     the digits of one radix-4096 number.  The multiplication is done 
c     piecemeal.
c     
c     It returns the product in the third argument.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer A, B, C, j1, j2, j3, j4, k1, k2, k3, k4
      integer a1,a2,a3,b1,b2,b3

      dimension A( IRandNumSize ),
     1     B( IRandNumSize ),
     2     C( IRandNumSize )

c------------------------------------------------------------------------------

c     write( *, 10 )
 10   format( ' ranfmodmult', /, '------------' )

c     j1 = A( 1 ) * B( 1 )
c     j2 = A( 1 ) * B( 2 ) + A( 2 ) * B( 1 )
c     j3 = A( 1 ) * B( 3 ) + A( 2 ) * B( 2 ) + A( 3 ) * B( 1 )
c     j4= A( 1 ) * B( 4 ) + A( 2 ) * B( 3 ) + A( 3 ) * B( 2 ) + A( 4 ) * B( 1 )
      a1 = A(1)
      a2 = A(2)
      a3 = A(3)
      b1 = B(1)
      b2 = B(2)
      b3 = B(3)
      j1 = a1 * b1
      j2 = a1 * b2 + a2 * b1
      j3 = a1 * b3 + a2 * b2 + a3 * b1
      j4 = a1 * B( 4 ) + a2 * b3 + a3 * b2 + A( 4 ) * b1

      k1 = j1
      k2 = j2 + k1 / 4096
      k3 = j3 + k2 / 4096
      k4 = j4 + k3 / 4096

      C( 1 ) = mod( k1, 4096 )
      C( 2 ) = mod( k2, 4096 )
      C( 3 ) = mod( k3, 4096 )
      C( 4 ) = mod( k4, 4096 )

c     write( *, 20 )
 20   format( '  leaving ranfmodmult' )

      return
      end
