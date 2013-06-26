C#############################################################################
C
C subroutine writeout       
C
C Writes summarized results. 
C Modification: Number of MPI tasks and OpenMP threads passed in from 
C sphot for efficiency calculation.  (bmb 3/12/01)
C
C#############################################################################

      subroutine writeout(numMPItasks, nThreads)
      include 'params.inc'
      include 'globals.inc'
      include 'shared.inc'
      include 'geomz.inc'
      include 'randseed.inc'
      include 'times.inc'

      integer nThreads, numMPItasks, ntracSum 
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

      real*8 escsum,escsumsq, escave,escstdev
      real*8 tsum,tsumsq,tave,tstd
      real*8 maxCopyGlobTime
      real*8 avg_enescSum

      INTEGER nout1
      INTEGER nout2

      parameter (nout1=11)
      parameter (nout2=12)


      if (print_flag .gt. 0) then
        open(nout1,FILE='out_full.txt',STATUS='UNKNOWN',
     |            ACCESS='SEQUENTIAL',FORM='FORMATTED')


      do 2000 i = 1, NRuns
         npart  = ng_npart(i) 
         nphtot = ng_nphtot(i)
         nploss = ng_nploss(i)
         etot   = g_etot(i)  
         epgain = g_epgain(i)
         nlost  = ng_nlost(i) 
         nesc   = ng_nesc(i)  
         nrr    = ng_nrr(i)   
         ncen   = ng_ncen(i)  
         nabs   = ng_nabs(i)  
         wlost  = g_wlost(i) 
         wesc   = g_wesc(i)  
         wrr    = g_wrr(i)   
         wcen   = g_wcen(i)  
         wabs   = g_wabs(i)  
         nscat  = ng_nscat(i) 
         nsplt  = ng_nsplt(i) 
         ntrac  = ng_ntrac(i)  
         texec  = g_texec(i)
         do 903 j = 1,negrps
            nescgp(j) = ng_nescgp(i,j)
            enesc(j)  = g_enesc(i,j) 
 903     continue

         CALL wroutput(i,tgen,texec,nout1)
 
 2000 continue

      endif



      ! CALL second(progEndTime)

      loopWallClockTime = loopEndTime - loopBeginTime
      progWallClockTime = progEndTime - progBeginTime

      loopTotalCPUTime = 0.0D0
      progTotalCPUTime = loopBeginTime - progBeginTime +
     .     progEndTime   - loopEndTime


      if (print_flag .gt. 0) then

      escsum   = 0.0d0
      escsumsq = 0.0d0
      tsum     = 0.0d0
      tsumsq   = 0.0d0
      ovrhsum  = 0.0d0
      ovrhsumsq= 0.0d0
      partSum = 0.0d0
      partSumSq = 0.0d0
      tracSum   = 0.0d0
      tracSumSq = 0.0d0


c      print *, "numMPItasks = ", numMPItasks
c      print *, "loopTime = ", loopWallClockTime
c      print *, "progTime = ", progWallClockTime

      write(nout1,500) numMPItasks
 500  format(//,'numMPItasks = ', i8)
      write(nout1,501) loopWallClockTime
 501  format('loopTime = ', f10.6)
      write(nout1,502) progWallClockTime
 502  format('progTime = ', f10.6)     


      write(nout1,9905)
 9905 format(//////,'SUMMARY:',/,'-------',/,
     1     'Run #     # part.    # tracks  esc. prob.',
     2     '     time  1st ranf',' last ranf'
     3     )
      do 1100 i= 1, NRuns 

         loopTotalCPUTime = loopTotalCPUTime + g_texec(i)
         progTotalCPUTime = progTotalCPUTime + g_texec(i)

         escsum   = escsum + g_ffesc(i)

         tsum     = tsum + g_texec(i)

         ovrhsum  = ovrhsum + g_overheadTime(i)

         partSum = partSum + ng_nphtot(i)

         tracSum     = tracSum + ng_ntrac(i)

         write(nout1,9904) i, ng_nphtot(i), ng_ntrac(i), g_ffesc(i),
     .        g_texec(i), g_firstRanf(i), g_lastRanf(i)

 9904    format(i8,i9,i12,f12.6,f9.4,f10.7,f10.7)
 1100 continue


      write(nout1,9979)
 9979 format('-------------------------------------------',
     1     '------------------------')
      escave = escsum / NRuns 
      tave     = tsum / NRuns
      ovrhave  = ovrhsum / NRuns
      iparticleAve = partSum / NRuns
      itracAve = tracSum / NRuns

      do 1200 i = 1, NRuns
         escsumsq = escsumsq + abs(g_ffesc(i)-escave) ** 2
         tsumsq   = tsumsq + abs(g_texec(i)-tave) ** 2
         ovrhsumsq = ovrhsumsq + abs(g_overheadTime(i)-ovrhave) ** 2
         partSumSq = partSumSq + abs(ng_nphtot(i)-iparticleAve) ** 2
         tracSumSq = tracSumSq + abs(ng_ntrac(i)-itracAve) ** 2
 1200 continue
      escstdev = sqrt(escsumsq/NRuns)
      tstd     = sqrt(tsumsq/NRuns)
      ovrhstd  = sqrt(ovrhsumsq/NRuns)
      ipartStDev = sqrt(partSumSq/NRuns)
      itracStDev = sqrt(tracSumSq/NRuns)
      partRatio  = ipartStDev / float(iparticleAve)
      tracRatio  = itracStDev / float(itracAve) 
      escRatio   = escstdev / escAve 
      tRatio     = tStd / tave

      write(nout1,9917) iparticleAve, itracAve, escAve, tave 
 9917 format('ave:    ',i9,i12,f12.6,f9.4)
      write(nout1,9918) ipartStDev, itracStDev, escStDev, tStd 
 9918 format('std:    ',i9,i12,f12.8,f9.4)
      write(nout1,9919) partRatio, tracRatio, escRatio, tRatio
 9919 format('ratio:   ',f8.5,f12.7,f12.8,f9.6)

      maxCopyGlobTime = copyGlobTime(1)
      do 1300 i = 2, numMPItasks
         if (maxCopyGlobTime .lt. copyGlobTime(i)) then
            maxCopyGlobTime = copyGlobTime(i)
         endif
 1300 continue

c...Sum of all execute() loops / 
      loopSpeedup = loopTotalCPUTime / loopWallClockTime

      singleCPUTime = progTotalCPUTime - allocateTime
     .     - seedGenTime - maxCopyGlobTime

      progSpeedup = singleCPUTime / progWallClockTime

      if (nThreads .eq. 0) then nThreads = 1
      efficiency = progSpeedUp / (numMPItasks * nThreads)

      write(nout1,9903) allocateTime, seedGenTime, maxCopyGlobTime,
     .     singleCPUTime, numMPItasks, nThreads,
     .     loopSpeedup, progSpeedup, efficiency
 9903 format(//,
     .     'allocate time:                      ',f15.6,/,
     .     'random number seed generation time: ',f15.6,/,
     .     'max. global copy time:              ',f15.6,/,
     .     'estimated single CPU time:          ',f15.2,/,
     .     'number of MPI tasks:                ',i15,   /,
     .     'number of threads per MPI task:     ',i15,   /,
     .     'loop speedup:                       ',f15.4,/,
     .     'program speedup:                    ',f15.4,/,
     .     'efficiency:                         ',f15.4
     .     )


         close(nout1)
      endif




      return
      end
