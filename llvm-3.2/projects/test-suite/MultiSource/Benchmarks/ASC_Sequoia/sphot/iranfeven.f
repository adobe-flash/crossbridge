c     ranfatok

c==============================================================================

      function iranfeven( N )

c------------------------------------------------------------------------------
c     
c     This function checks the parity of the argument integer.
c     
c     It returns one if the argument is even and zero otherwise.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer N

c------------------------------------------------------------------------------

      if( mod( N, 2 ) .eq. 0 ) then
         iranfeven = 1
      else
         iranfeven = 0
      endif

      return
      end
