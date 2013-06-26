      
c     
      function plnkut(u1,u2)
c***********************************************************************
c     function plnkut - evaluates the fraction of energy emitted in     *
c     region ir and group ig based on the plankian    *
c     integral.                                       *
c***********************************************************************
c     ******************************************************************
c     * plnkut(u1,u2) =                                                *
c     *    15.*pi**(-4)*integral(u**3/(exp(u)-1.),  u1 .lt. u .lt. u2) *
c     *                                                                *
c     *   plnkut(0,z) = sum (ck*z**k  ,  k=3,4,5,...)  near z = 0      *
c     *                                                                *
c     *   f(i) = plnkut(0,xm + h*i)                                    *
c     ******************************************************************
      
      implicit double precision (a-h,o-z)
      
      dimension f(59)
      data f  /.144005d0,.177286d0,.212769d0,.249946d0,.288322d0,
     x .327420d0,.366798d0,
     1     .406054d0,.444830d0,.482815d0,.519747d0,.555410d0,.589629d0,
     x     .622277d0,.653255d0,
     2     .682506d0,.709999d0,.735729d0,.759714d0,.781988d0,.802601d0,
     x     .821615d0,.839099d0,
     3     .855130d0,.869788d0,.883155d0,.895317d0,.906355d0,.916351d0,
     x     .925385d0,.933533d0,
     4     .940868d0,.947460d0,.953372d0,.958668d0,.963403d0,.967630d0,
     x     .971399d0,.974754d0,
     5     .977738d0,.980387d0,.982737d0,.984818d0,.986660d0,.988288d0,
     x     .989726d0,.990994d0,
     6     .992111d0,.993095d0,.993961d0,.994721d0,.995389d0,.995975d0,
     x     .996489d0,.996939d0,
     7     .997333d0,.997677d0,.997978d0,.998242d0/
      
      data c3,c4,c5,c7,c9/
     1     .51329911d-1,.19248717d-1,.25664956d-2, .30553519d-4,
     x     .5658059d-6/
      data xmin,xmax,q,xm,h/1.9d0,12.0d0,.15398973d0,1.62d0,.18d0/
      
c     ------------------------------------------------------------------
      
      if (u1 .le. xmax) then
c     *****************************************
c     * two passes will be made through loop. *
c     * first pass (iq=1) will set up for the *
c     * upper limit. the second pass (iq=2)   *
c     * will set up for the lower limit.      *
c     *****************************************
         iq = 1
         z  = u2
         
 87      if ( iq .ge. 3 ) go to 88
         
         if (z .gt. xmax) then
c     *****************************************
c     * use asymptotic formula for z.ge.xmax  *
c     *****************************************
            plkint = 1.-q*exp(-z)*((z+3.)*z+6.)*z
            
         else if (z .gt. xmin) then
c     *****************************************
c     * quadratic interpolation for           *
c     *         xmin .lt. z .lt. xmax         *
c     *****************************************
            d      = (z-xm)/h
            i      = d
            d      = d-dble(i)
            dm     = d -1.d0
            dn     = dm-1.d0
            plkint = (f(i)*dm*dn+f(i+2)*d*dm)*.5d0-f(i+1)*dn*d
            
         else
c     *****************************************
c     * use power series for  z .le. xmin     *
c     *****************************************
            
            plkint = ((((c9*(z*z)-c7)*(z*z)+c5)*z-c4)*z+c3)*z*(z*z)
         endif
         
         if (iq .eq. 1) then
            z      = u1
            plnkut = plkint
         endif
         
         iq = iq+1
         go to 87
         
 88      continue
         plnkut = plnkut - plkint
         
      else
         zu = u2
         zl = u1
         fstlog = log(zl*(6.+zl*(3.+zl)))
         sndlog = log(zu*(6.+zu*(3.+zu)))
         plnkut = q * (exp(fstlog-zl) - exp(sndlog-zu))
      endif
      
      return
      end
