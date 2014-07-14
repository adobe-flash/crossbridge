
      
      subroutine wroutput (iRun,tgen,texec,nout1)
c----------------------------------------------------------------------
c     Print out the final results.
c----------------------------------------------------------------------
      include 'params.inc'
      include 'geomz.inc'
      include 'globals.inc'
      include 'shared.inc'

      integer*4 iRun

      INTEGER*4 nescgp(negrps)
      real*8 enesc(negrps)
      real*8 wcut, wmin, wmax
      real*8 wlost, wesc, wrr
      real*8 wabs, wcen, epgain, etot
      integer*4 npart, nphtot, nploss
      integer*4 nlost, nesc, nrr, nabs, ncen
      integer*4 nscat, nsplt, ntrac
      common /mjcstuff/  nescgp, enesc, wcut, wmin, wmax,
     &     wlost, wesc, wrr, wabs, wcen, epgain, etot, 
     &     npart, nphtot, nploss, nlost, nesc, nrr, nabs,
     &     ncen, nscat, nsplt, ntrac

      INTEGER nout1
      
c------------------------------print out final results



      if (print_flag .gt. 1) then
      write(nout1,209) iRun
 209  format('Run number: ',i4,//)
      endif

      npart = nphtot

      if (print_flag .gt. 1) then
      write (nout1,210)
 210  format(/20x,'plankian source:',
     .     / 2x,'# emitted',
     .     3x,'# killed',
     .     3x,'energy emitted (kev)'
     .     ,3x,'energy gained via rr')
      
      write (nout1,211) npart, nploss, etot, epgain
 211  format(1x,i8,4x,i8,5x,e12.4,12x,e12.4)
      endif
      
      trials = dble(npart)
      do 498 i=1,12
         wesc = wesc + enesc (i)
         nesc = nesc + nescgp(i)
 498  continue
      ffesc = wesc/trials
      g_ffesc(iRun) = ffesc
      std   = sqrt(ffesc*(1.0 - ffesc)/trials)

      if (print_flag .gt. 1) then      
      write (nout1,251) nabs, nesc, ntrac, nlost, ncen, nscat, nrr, nsplt
 251  format(/'nabs   = ',i10,
     |     /'nesc   = ',i10,
     |     /'ntrac  = ',i10,
     |     /'nlost  = ',i10,
     |     /'ncen   = ',i10,
     |     /'nscat  = ',i10,
     |     /'nrr    = ',i10,
     |     /'nsplit = ',i10)
      
      write (nout1,252) ffesc, std
 252  format(/' escape prob = ',f10.6,
     .     /' std =         ',f10.6)
      
      write (nout1,258)
 258  format(/1x,'energy group',5x,'# bundles escaping',5x,'energy (kev)
     |     ')
      write (nout1,259) (ig,nescgp(ig),enesc(ig)*bwgt, ig=1,12)
 259  format(4x,i4,15x,i6,13x,e10.4)
      
      write (nout1,271)
 271  format(/20x,'energy balance (kev):',/3x,'emitted',4x,'lost via rr'
     |     ,4x,'absorbed',5x,'escaped',5x,'censused',7x,'lost')
      endif
     
      err = epgain + wrr*bwgt


      if (print_flag .gt. 1) then   
      write (nout1,272) etot, err, wabs*bwgt, wesc*bwgt, wcen*bwgt,
     |     wlost*bwgt
 272  format(5(e11.4,2x),e11.4/)
      endif

      
      ttrack = 1.0d+6 * texec / dble(ntrac)

      if (print_flag .gt. 1) then  
      write(nout1,253) tgen, texec, ntrac, ttrack
 253  format(/5x,'generation time (sec)  = ',e15.7
     +     /5x,'execution time  (sec)  = ',e15.7
     +     //5x,'number of tracks       = ',i10,
     +     /5x,'time/track (microsec)  = ',e15.7)

      write(nout1,9902)
 9902 format('-------------------------------------------------')
      endif

      
      return
      end
