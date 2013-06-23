
      subroutine execute ( 
     &        myIter, myStream, mySeed, nescgp, enesc, wcut, 
     &        wmin, wmax, wlost, wesc, wrr, wabs, wcen, epgain, etot,
     &        npart, nphtot, nploss, nlost, nesc, nrr, nabs, ncen,
     &        nscat, nsplt, ntrac, firstRanf, depArray  )
cc----------------------------------------------------------------------
c     Generate source particles and track them.
c     Outer loop is over zones, inner loop is over energy groups
cc----------------------------------------------------------------------
      include 'params.inc'
      include 'geomz.inc'
      include 'globals.inc'
      
      integer*4 myIter, myStream, mySeed(*)
      INTEGER*4 nescgp(negrps)
      double precision enesc(negrps)
      double precision wcut, wmin, wmax
      double precision wlost, wesc, wrr
      double precision wabs, wcen, epgain, etot
      integer*4 npart, nphtot, nploss
      integer*4 nlost, nesc, nrr, nabs, ncen
      integer*4 nscat, nsplt, ntrac
      double precision firstRanf
      double precision depArray(nzmax)

      logical itest(4), jtest(4), ktest, noscat, isign
      double precision xm(4), xb(4), del(4), lmin, l1, l2, l, newgt
      double precision xkt4 (nmrmax), efrac (nmrmax,negp1)
      
c.... save arrays
      double precision xsv(nsvmax), ysv(nsvmax), zsv(nsvmax),
     &     usv(nsvmax), vsv(nsvmax), wsv(nsvmax), agesv(nsvmax),
     &     wgtsv(nsvmax), idsv(nsvmax), irsv(nsvmax)

      double precision hnu(negp1)

      data hnu/.001d0,0.5d0,1.0d0,2.0d0,3.5d0,4.75d0,6.0d0,7.5d0,9.0d0,
     |     12.0d0,20.0d0,80.0d0,200.0d0/
      
      double precision ranf
      external ranf
c***
c     use common block to fool the compiler so that it will not
c     optimize out the dummy code inside the critical region

      real*8 DummyArray(10)

      common / DUMMY / DummyArray  
      
c***  new source segment to be used with plankian - loop over cells
c     planc returns zone indices, groups, emission times, and weights for
c     all particles, assuming planckian emission for each zone
      
c***  efrac(ir,ig) = fraction of energy emitted in region ir that
c     appears in group ig
      
c---------- particles generated from source term, (i.e., temperature)
c---------- store plnkut results per region
c---------- also tmp is in ev, need kev
      
      nlost = 0                 ! Initialize private common variables.
      nesc  = 0
      nrr   = 0
      ncen  = 0
      nabs  = 0
      wlost = 0.0d0
      wesc  = 0.0d0
      wrr   = 0.0d0
      wcen  = 0.0d0
      wabs  = 0.0d0
      nscat = 0
      nsplt = 0
      ntrac = 0

      do 100 ijkl = 1,negrps
         nescgp (ijkl) = 0
         enesc  (ijkl) = 0.0d0
 100  continue

      CALL copyseed ( myIter, myStream, mySeed, firstRanf )

      const  = 2.567189267958d33 * tcen
      do 300 ir = 1,nreg
         tempk    = 1.0d-3 * tmp(ir)
         xkt4(ir) = tempk*tempk*tempk*tempk*const
         xtemp    = 1.0d0 / tempk
         do 301 ig = 1,12
            u1 = hnu(ig  ) * xtemp
            u2 = hnu(ig+1) * xtemp
            efrac(ir,ig) = plnkut(u1,u2)*xkt4(ir)*sigtot(ir,ig)
 301     continue
 300  continue
      
      vel    = 2.99793d10
c---------- initialize some GLOBAL diagnostic variables
      nploss = 0
      epgain = 0.0d0
      etot   = 0.0d0
      nphtot = 0
      

c-------------------- LOOP OVER ZONES BEGINS

      do 400 iz = 1, nzones

c-------------------- determine if sides are reversed for rejection sampling
         
         itest(1) = rr(iz,1) .lt. rr(iz,2)
         itest(2) = zz(iz,2) .lt. zz(iz,3)
         itest(3) = rr(iz,4) .lt. rr(iz,3)
         itest(4) = zz(iz,1) .lt. zz(iz,4)

c-------------------- determine minimum and maximum coordinates for sampling
         rmin = rr(iz,1)
         rmax = rr(iz,1)
         zmin = zz(iz,1)
         zmax = zz(iz,1)
         do 88 ii = 2,4
            rrtemp = rr(iz,ii)
            zztemp = zz(iz,ii)
            if (rrtemp .gt. rmax) rmax = rrtemp
            if (rrtemp .lt. rmin) rmin = rrtemp
            if (zztemp .gt. zmax) zmax = zztemp
            if (zztemp .lt. zmin) zmin = zztemp
 88      continue

         rminsq = rmin*rmin
         rsq    = rmax*rmax - rminsq
         zmax   = zmax - zmin			! should be called zdelta
         
c-------------------- determine parameters for intersection calculation with zone sides

         if (itype(iz,1) .ne. 2) then
            del(1) = rr(iz,1) - rr(iz,2)
            xm (1) = (zz(iz,1) - zz(iz,2))/del(1)
            xb (1) = (zz(iz,2)*rr(iz,1) - zz(iz,1)*rr(iz,2))/del(1)
         endif
         
         del(2) = zz(iz,3) - zz(iz,2)
         if (del(2) .ne. 0.0) then
            xm(2) = (rr(iz,3) - rr(iz,2))/del(2)
            xb(2) = (zz(iz,3)*rr(iz,2) - zz(iz,2)*rr(iz,3))/del(2)
         endif
         
         if (itype(iz,3) .ne.2) then
            del(3) = rr(iz,4) - rr(iz,3)
            xm (3) = (zz(iz,4) - zz(iz,3))/del(3)
            xb (3) = (rr(iz,4)*zz(iz,3) - rr(iz,3)*zz(iz,4))/del(3)
         endif
         
         del(4) = zz(iz,4) - zz(iz,1)
         xm (4) = (rr(iz,4) - rr(iz,1))/del(4)
         xb (4) = (zz(iz,4)*rr(iz,1) - zz(iz,1)*rr(iz,4))/del(4)


c------------------- now begin the inner loop over energy groups
         
         do 150 ig = 1, 12

            ir   = mid (iz)
            edep = efrac(ir,ig) * volcl(iz)
            
c***  units:  tcen   (s),
c     sigtot (1/cm),
c     volcl  (cc),
c     xkt4   (kev),
c     edep   (kev)
c     efrac  (kev/cc)
c     bwgt = input bundle size,
c     xpht = # of photons (non-integer),
c     npht = truncated # of photons (rounded up or down depending on random #)
            
            etot = etot + edep
            xpht = edep / bwgt
            npht = xpht + ranf (mySeed)

            if (npht .le. 0) then
c------------------- when xpht is truncated to zero, keep track of:
c                    energy (epgain) and number lost (nploss)
               epgain = epgain - edep
               nploss = nploss + 1
               go to 150
            endif
            
c-------------------- calculate bundle weight
            phwgt = edep / (bwgt*npht)
            
c-------------------- modify weight to account for rejected bundles
            if (xpht .lt. 1.0d0) then
               phwgt  = phwgt / xpht
               epgain = epgain + edep * ((1.0/xpht) - 1.0d0)
            endif
            nphtot = nphtot + npht


c***  now loop over the number of photons emitted in zone iz and group ig
            do 159 k = 1, npht

c***
c     insert dummy critical region

!$OMP       critical

c            if (ranf(mySeed) .le. 0.05) then
c                DummyArray(10)    = xsv  (isplt)
c                DummyArray(10)    = ysv  (isplt)
c                DummyArray(10)    = zsv  (isplt)
c                DummyArray(10)    = usv  (isplt)
c                DummyArray(10)    = vsv  (isplt)
c                DummyArray(10)    = wsv  (isplt)
c                DummyArray(10)    = agesv(isplt)
c                DummyArray(10)    = wgtsv(isplt)
           if (phwgt .le. 0.05) then
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0
                DummyArray(10)    = 0.0

            end if               

!$OMP       end critical



               isplt  = 0
               ibegin = 1
               iend   = 0
               newgt  = phwgt
               age    = ranf(mySeed) * tcen
               ir     = mid(iz)
               
c------------------- pick direction cosines
               
               w      = 2.0d0*ranf(mySeed) - 1.0
               t3     = 6.2831853d0 * ranf(mySeed)
               snthet = sqrt(1.0d0 - w*w)
               u      = cos(t3) * snthet
               v      = sin(t3) * snthet
               
c------------------- sample a point uniformly in the zone (rejection method)
c                    to create an initial position
               
 119           rsamp = sqrt (rsq  * ranf (mySeed) + rminsq)
               zsamp =       zmax * ranf (mySeed) + zmin

c***  determine if the point is inside the zone, if not reject
c     1=right, 2=bottom, 3=left, 4=top
               
               if (itype(iz,1) .ne. 2) then
                  jtest(1) = zsamp .lt. (xm(1)*rsamp + xb(1))
               else
                  jtest(1) = .not.itest(1)
               end if
               
               ktest = ((itest(1) .and. jtest(1)) .or.
     |              (.not.itest(1) .and. .not.jtest(1)))
               if (ktest) go to 119                       ! outside, go sample again

               if (del(2) .ne. 0.0) then
                  jtest(2) = rsamp .gt. (xm(2)*zsamp + xb(2))
               else
                  jtest(2) = .not.itest(2)
               end if
               
               ktest = ((itest(2) .and. jtest(2)) .or.
     |              (.not.itest(2) .and. .not.jtest(2)))
               if (ktest) go to 119                       ! outside, go sample again
               
               if (itype(iz,3) .ne.2) then
                  jtest(3) = zsamp .gt. (xm(3)*rsamp + xb(3))
               else
                  jtest(3) = .not.itest(3)
               end if
               ktest = ((itest(3) .and. jtest(3)) .or.
     |              (.not.itest(3) .and. .not.jtest(3)))
               if (ktest) go to 119                       ! outside, go sample again
               
               jtest(4) = rsamp .lt. (xm(4)*zsamp + xb(4))
               ktest    = ((itest(4) .and. jtest(4)) .or.
     |              (.not.itest(4) .and. .not.jtest(4)))
               if (ktest) go to 119                       ! outside, go sample again

c------------------- once inside all four zone sides, continue to tracking section
               
               t3 = 6.2831853d0 * ranf (mySeed)
               y  = rsamp * sin(t3)				! convert to 3D cartesian coordinates
               x  = rsamp * cos(t3)
               z  = zsamp
               
c------------------- BEGIN TRACKING SECTION
               id    = iz							 ! id = zone # as a particle moves from zone to zone
 45            l     = 1000.0d0
               ntrac = ntrac + 1					 ! increment the track count
               
               do 4 ks = 1, 4						 ! check for intersection with each of four sides
                                                     ! and find minimum distance                 
                  lmin = 1000.0 ! large initial min.
				  
                  itypeidks = itype (id, ks)
                  if (itypeidks .eq. 1) go to 42     ! normal zone side
                  if (itypeidks .eq. 2) go to  4     ! zone side on z axis, skip to next side
                  if (itypeidks .eq. 3) go to 30     ! horizontal zone side
                  if (itypeidks .eq. 4) go to 10     ! vertical zone side
                  
c------------------- normal zone side
 42               c1 = z + bom(id,ks)
                  c2 = sqm(id,ks)*c1
                  aa = 1.0d0 - (1.0d0 + sqm(id,ks))*w*w
                  bb = x*u + y*v - c2*w
                  cc = x*x + y*y - c2*c1
                  if (aa .eq. 0.0d0) then
                     lmin = - cc / bb
                     go to 44                ! to test against minimum intersection distance
                  endif
                  
                  disc = bb*bb - aa*cc
                  if (disc .lt. 0.0) go to 4     ! no real solution, => no intersection, => skip
                  d = sqrt(disc)
                  
                  l1 = (d - bb)/aa
                  if (l1 .ge. 1.0d-10) then
                     z1     = z + w*l1
                     zzidks = zz(id,ks)
                     zzidks1= zz(id,ks+1)
                     isign  = (z1.lt.zzidks  .and. z1.lt.zzidks1 ) .or.
     |                        (z1.gt.zzidks1 .and. z1.gt.zzidks  )
                     if (isign) l1 = 1000.0d0
                     if (l1 .lt. lmin) lmin = l1
                  endif
                  
                  l2 = (-bb - d)/aa
                  if (l2 .ge. 1.0d-10) then
                     z2     = z + w*l2
                     zzidks = zz(id,ks)
                     zzidks1= zz(id,ks+1)
                     isign  = (z2.lt.zzidks  .and. z2.lt.zzidks1 ) .or.
     |                        (z2.gt.zzidks1 .and. z2.gt.zzidks  )
                     if (isign) l2 = 1000.0d0
                     if (l2 .lt. lmin) lmin = l2
                  endif
                  go to 44                ! to test against minimum intersection distance
                  
c------------------- vertical zone side
 10               lmin = (zz(id,ks) - z) / w
                  x1   = x + u*lmin
                  y1   = y + v*lmin
                  r1   = sqrt(x1*x1 + y1*y1)
                  isign =  (r1.lt.rr(id,ks  ) .and. r1.lt.rr(id,ks+1))
     |                .or. (r1.gt.rr(id,ks+1) .and. r1.gt.rr(id,ks  ))
                  if (isign) lmin = 1000.0d0
                  go to 44                ! to test against minimum intersection distance
                  
c------------------- horizontal zone side
 30               aa   = 1.0d0 - w*w
                  bb   = x*u + y*v
                  cc   = x*v - y*u
                  disc = aa*rr(id,ks)*rr(id,ks) - cc*cc
                  if (disc .lt. 0.0d0) go to 4
                  d  = sqrt(disc)
                  
                  l1 = (d - bb)/aa
                  if (l1 .ge. 1.0d-10) then
                     z1 = z + w*l1
                     isign = (z1.lt.zz(id,ks  ) .and. z1.lt.zz(id,ks+1))
     |                  .or. (z1.gt.zz(id,ks+1) .and. z1.gt.zz(id,ks  ))
                     if (isign) l1 = 1000.0d0
                     if (l1 .lt. lmin) lmin = l1
                  endif
                  
                  l2 = (-bb - d)/aa
                  if (l2 .ge. 1.0d-10) then
                     z2 = z + w*l2
                     isign = (z2.lt.zz(id,ks  ) .and. z2.lt.zz(id,ks+1)) 
     |                  .or. (z2.gt.zz(id,ks+1) .and. z2.gt.zz(id,ks  ))
                     if (isign) l2 = 1000.0d0
                     if (l2 .lt. lmin) lmin = l2
                  endif
                  go to 44                ! to test against minimum intersection distance
                  
 44               if ((lmin .le. l    ) .and. 
     |                (lmin .gt. 0.0d0)) then      ! new minimum distance found
                     l     = lmin                  ! remember the distance
                     iside = ks                    ! and the side of closest intersection
                  endif
                  
 4             continue          ! end of section testing intersection with edges

c----- DETERMINE THE FATE OF THE PARTICLE:
c      absorbed, escaped, moved, scattered or censused
               
               if (l .eq. 1000.0d0) then      ! no intersection found!  => LOST
                  nlost = nlost + 1
                  wlost = wlost + newgt
                  go to 15                    ! to check for saved photon to track
               endif
               
               dist = -log(ranf(mySeed)) / sig(ir,ig)     ! distance to collision
               dcen = (tcen - age) * 2.99793d10           ! distance to census (i.e., end of time step)
               
               if (l .lt. dist .and. l .lt. dcen) go to 26 ! move to the boundary intersection
               
               if (dist .lt. dcen) then
                  noscat = ranf(mySeed) .gt. scrat(ir,ig)  ! probability of scattering vis. absorption
                  if (noscat) then
c------------------------------absorption
                     nabs = nabs + 1
                     wabs = wabs + newgt
c.....DEPOSITION.....
!$OMP ATOMIC
                     depArray(id) = depArray(id) + newgt   ! deposit absorbed energy in the zone

c.....END DEPOSITION

                  else
c------------------------------Thomson scattering
                     nscat = nscat + 1				  ! count the scatters
                     x = x + u*dist					  ! move in 3D to the point of scattering
                     y = y + v*dist
                     z = z + w*dist
                     age = age + dist*3.3356349d-11   ! advance the "age" of the photon
                     CALL thom (u, v, w, mySeed)
					                                  ! 
					 
                     if (irr .eq. 2) go to 525        ! rr/splitting via relative bundle size
                     go to 45                         ! to continue tracking
                  end if
               else
c------------------------------Census time. Fake putting the particle into storage

!$OMP       critical

						DummyArray(10)    = x
						DummyArray(10)    = y
						DummyArray(10)    = z
						DummyArray(10)    = u
						DummyArray(10)    = v
						DummyArray(10)    = w
						DummyArray(10)    = age
						DummyArray(10)    = newgt

!$OMP       end critical

                  ncen = ncen + 1
                  wcen = wcen + newgt

               end if
               go to 15                               ! to check for saved photon to track
               
c-------------------------------------------------- MOVE THE PARTICLE TO THE CORRECT BOUNDARY
 26            idold = id
               id    = id + ng_incr(iside)			  ! zone ID changed according to which side was crossed

               if ( id .gt. nzones ) then
c-------------------------------------------------- Escaped
                  nescgp(ig) = nescgp(ig) + 1
                  enesc (ig) = enesc (ig) + newgt
                  go to 15                            ! to check for saved photon to track
               endif
               
               ir  = mid (id)
               x   = x + u*l   ! what about right on the side?
               y   = y + v*l
               z   = z + w*l
               age = age + l*3.3356349d-11			  ! distance divided by the speed of light
               
c--------------------russian roulette/splitting options
               
c     irr = 0  no rr/splitting
c     irr = 1  rr/splitting via relative zone importances
c     irr = 2  rr/splitting via relative bundle size
               
               if (irr .eq. 0) go to 45                ! to continue tracking
               if (irr .eq. 2) go to 525			   ! rr/splitting via relative bundle size
               
c-------------------- rr/splitting via zone importances
c					  (only applies to particles which cross zone boundaries)
               
               if (ximp(id) .eq. ximp(idold)) go to 45  ! NO change in zone importance. => continue tracking
               r = ximp(id) / ximp(idold)
               
               zeta = ranf(mySeed)
               ir1  = r
               r1   = dble(ir1)
               ir0  = ir1 - 1
               r0   = r - r1
               
               if (r .lt. 1.0d0) then
c------------------------------russian roulette
                  if (zeta .lt. r0) then
c------------------------------ survive
                     rinv  = 1.0d0 / r
                     wrr   = wrr + newgt * (rinv - 1.0d0)
                     newgt = newgt * rinv
                     go to 45                    ! to continue tracking
                  endif
c------------------------------ kill
                  nrr = nrr + 1
                  wrr = wrr - newgt
                  go to 15                   ! to check for saved photon to track
               endif
               
c------------------------------ split
               if (zeta .lt. r0) then
c------------------------------ split high
                  newgt = newgt / (r1 + 1.0d0)
                  isplt = isplt + ir1
                  nsplt = nsplt + ir1
                  go to 415                  ! to save new photon in storage
               endif
c-----------------------------split low
               if (ir0 .le. 0) go to 45      ! to continue tracking
               r1    = dble(ir0)
               isplt = isplt + ir0
               nsplt = nsplt + ir0
               newgt = newgt / (r1 + 1.0d0)
               go to 415                     ! to save new photon in storage
               
c------------------------------ rr/splitting via relative bundle size
 525           r = newgt
               if (r .lt. wmin) then
c------------------------------russian roulette
                  if (ranf(mySeed) .lt. r) then
c------------------------------survive
                     wrr = wrr + 1.0d0 - r
                     newgt = 1.0d0
                     go to 45                 ! to continue tracking
                  endif
c------------------------------kill
                  nrr = nrr + 1
                  wrr = wrr - newgt
                  go to 15                    ! to check for saved photon to track
               endif
c------------------------------split
               if (r .lt. wmax) go to 45      ! to continue tracking
               ir0 = r
               ir1 = ir0 - 1
               if (ir1 .le. 0) go to 45       ! to continue tracking
			   
               r1    = dble(ir1)
               newgt = newgt/(r1 + 1.0)
               isplt = isplt + ir1
               nsplt = nsplt + ir1
               
c------------------------------save position, direction, age, zone and weight
c                              of cloned particles
 415           iend = isplt
               do 317 i1 = ibegin,iend
                  xsv  (i1) = x
                  ysv  (i1) = y
                  zsv  (i1) = z
                  usv  (i1) = u
                  vsv  (i1) = v
                  wsv  (i1) = w
                  agesv(i1) = age
                  wgtsv(i1) = newgt
                  idsv (i1) = id
                  irsv (i1) = ir
 317           continue
               
               ibegin = iend + 1
               go to 45                   ! to continue tracking
               
 15            if (isplt .ne. 0) then
c------------------------------now dispense split particles from group ig
                  x      = xsv  (isplt)
                  y      = ysv  (isplt)
                  z      = zsv  (isplt)
                  u      = usv  (isplt)
                  v      = vsv  (isplt)
                  w      = wsv  (isplt)
                  age    = agesv(isplt)
                  newgt  = wgtsv(isplt)
                  id     = idsv (isplt)
                  ir     = irsv (isplt)
                  isplt  = isplt - 1
                  ibegin = ibegin - 1
                  go to 45                ! to continue tracking
               endif
               
 159        continue	! end of loop over the number of photons emitted in zone iz and group ig
            
 150     continue		! end of loop over energy groups

 400  continue			! end of loop over zones

      return
      end
