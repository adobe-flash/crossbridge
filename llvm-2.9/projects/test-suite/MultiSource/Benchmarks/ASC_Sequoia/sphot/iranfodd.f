c     end ranfmodmult

c==============================================================================

      function iranfodd( N )

c------------------------------------------------------------------------------
c     
c     This function checks the parity of the argument integer.
c     
c     It returns one if the argument is odd and zero otherwise.
c     
c------------------------------------------------------------------------------

      include 'pranf.inc'

      integer N

c------------------------------------------------------------------------------

      if( mod( N, 2 ) .eq. 1 ) then
         iranfeven = 0
      else
         iranfeven = 1
      endif

      iranfodd = 1 - iranfeven
      return
      end
