      
      subroutine zonevols
c***********************************************************************
c     calculates absolute cell volumes (cc) needed for plankian source.                           *
c***********************************************************************
c     implicit double precision (a-h,o-z)
      include 'params.inc'
      include 'geomz.inc'
      
      do 20 j = 1,nzones
         g_volcl(j) = 0.0d0
 20   continue
      
      do 21 ks = 1,4
         do 22 j = 1,nzones
            if (ng_itype(j,ks) .ne. 2 .and. ng_itype(j,ks) .ne. 4) then
               tv1 = (g_zz(j,ks+1) - 
     .              g_zz(j,ks))*(g_rr(j,ks+1)*(g_rr(j,ks+1)
     .              + g_rr(j,ks)) + g_rr(j,ks)*g_rr(j,ks))
               g_volcl(j) = g_volcl(j) + 1.0471976*tv1
            endif
 22      continue
 21   continue
      
      return
      end
