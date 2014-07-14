      
      subroutine interp (matl,dnt,temp,opacity)
c***********************************************************************
c     calculates material opacities as a function of temperature and
c     density by interpolating in the llnl library.
c     arguments:
c     matl: material number
c     temp: temperature (units?),
c     dnt :  density (units?)
c     returns: 13 group opacity vector into array opacity.
c***********************************************************************
      implicit double precision (a-h,o-z)
      
      common /opactab/ x(14,32,13,4), t(32,2), d(14)
      dimension opacity(13)
c     scatter, shared opacity


      ityp = 1
      if (matl .eq. 2 .or. matl .eq. 4) ityp = 2
      
c--------------------find bounding temperatures
      do 20 it = 1,32
         if (temp .lt. t(it,ityp)) goto 30
 20   continue
 30   tmax = t(it,ityp)
      it1 = it-1
      tmin = t(it1,ityp)
      trat = (temp - tmin)/(tmax - tmin)
      
c--------------------find bounding densities
      do 40 id = 1,14
c         if (dnt .lt. d(id)) goto 50   Replaced with following due to 
c addressing problems
         if (dnt .le. d(id)) goto 50
 40   continue
 50   dmax = d(id)
      id1 = id-1
      dmin = d(id1)
      drat = (dnt - dmin)/(dmax - dmin)
      
c--------------------interpolate
      do 60 ie = 1,13
         xt = x(id1,it,ie,matl) + (x(id,it,ie,matl)-x(id1,it,ie,matl))
     |        * drat
         xt1 = x(id1,it1,ie,matl)
     |        + (x(id,it1,ie,matl)-x(id1,it1,ie,matl))*drat
         opacity (ie) = exp(xt1 + (xt-xt1)*trat)
 60   continue
      
      return
      end
