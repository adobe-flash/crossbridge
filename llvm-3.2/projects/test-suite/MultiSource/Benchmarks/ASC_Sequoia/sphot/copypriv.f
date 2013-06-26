
      
      subroutine copypriv(ithRun, srFlag,  nescgp, enesc, wcut, 
     &     wmin, wmax, wlost, wesc, wrr, wabs, wcen, epgain, etot,
     &     npart, nphtot, nploss, nlost, nesc, nrr, nabs, ncen,
     &     nscat, nsplt, ntrac, 
     &     g_etot, g_epgain, g_wlost, g_wesc, g_wrr, g_wabs, 
     &     g_wcen, g_enesc, ng_npart, ng_nphtot, ng_nploss,
     &     ng_nlost, ng_nesc, ng_nrr, ng_nabs, ng_ncen, ng_nscat,
     &     ng_nsplt, ng_ntrac, ng_nescgp, ibuf, jbuf, rbuf, sbuf,
     &     lescbuf, nescbuf, lenbuf, enbuf )


      include 'params.inc'
      include 'globals.inc'
c     include 'shared.inc'
      include 'geomz.inc'
      include 'times.inc'
      include 'mpif.h'
     
      INTEGER ithRun
      LOGICAL srFlag
      INTEGER*4 nescgp(negrps)
      real*8 enesc(negrps)
      real*8 wcut, wmin, wmax
      real*8 wlost, wesc, wrr
      real*8 wabs, wcen, epgain, etot
      integer*4 npart, nphtot, nploss
      integer*4 nlost, nesc, nrr, nabs, ncen
      integer*4 nscat, nsplt, ntrac

      real*8   g_etot(maxruns) 
      real*8   g_epgain(maxruns)
      real*8   g_wlost(maxruns) 
      real*8   g_wesc(maxruns) 
      real*8   g_wrr(maxruns) 
      real*8   g_wabs(maxruns) 
      real*8   g_wcen(maxruns)
      real*8   g_enesc (maxruns,negrps)

      integer*4   ng_npart(maxruns) 
      integer*4   ng_nphtot(maxruns) 
      integer*4   ng_nploss(maxruns)
      integer*4   ng_nlost(maxruns) 
      integer*4   ng_nesc(maxruns) 
      integer*4   ng_nrr(maxruns) 
      integer*4   ng_nabs(maxruns) 
      integer*4   ng_ncen(maxruns)
      integer*4   ng_nscat(maxruns) 
      integer*4   ng_nsplt(maxruns) 
      integer*4   ng_ntrac(maxruns)
      integer*4   ng_nescgp(maxruns,negrps)

      INTEGER OMP_GET_THREAD_NUM, OMP_GET_NUM_THREADS
      EXTERNAL OMP_GET_NUM_THREADS, OMP_GET_THREAD_NUM

      INTEGER numThreads, ierr, MPIid,    thrID,
     &        stat(MPI_STATUS_SIZE), 
     &        req(maxMPItasks),
     &        statArray(MPI_STATUS_SIZE, maxMPItasks),
     &        ibuf(maxThreadsPerMPItask*11), 
     &        jbuf(maxThreadsPerMPItask*11, maxMPItasks),
     &        lescbuf(maxThreadsPerMPItask*negrps), 
     &        nescbuf(maxThreadsPerMPItask*negrps, maxMPItasks)

      REAL*8  rbuf(maxThreadsPerMPItask*7), 
     &        sbuf(maxThreadsPerMPItask*7, maxMPItasks),
     &        lenbuf(maxThreadsPerMPItask*negrps), 
     &        enbuf(maxThreadsPerMPItask*negrps, maxMPItasks)


c...  Take all contributions to these global values and sum them onto the
c...  MPI task 0

      CALL MPI_COMM_RANK( MPI_COMM_WORLD, MPIid, ierr )
      CALL MPI_COMM_SIZE( MPI_COMM_WORLD, numMPItasks, ierr )

      numThreads = OMP_GET_NUM_THREADS()
      thrID = OMP_GET_THREAD_NUM()

c...  Just set the global arrays if I'm the master node

      if( MPIid .eq. 0 )then

         k = ithRun

         ng_npart(k)  = npart
         ng_nphtot(k) = nphtot
         ng_nploss(k) = nploss
         ng_nlost(k)  = nlost
         ng_nesc(k)   = nesc
         ng_nrr(k)    = nrr
         ng_ncen(k)   = ncen
         ng_nabs(k)   = nabs
         ng_nscat(k)  = nscat
         ng_nsplt(k)  = nsplt
         ng_ntrac(k)  = ntrac

         g_etot(k)   = etot
         g_epgain(k) = epgain
         g_wlost(k)  = wlost
         g_wesc(k)   = wesc
         g_wrr(k)    = wrr
         g_wcen(k)   = wcen
         g_wabs(k)   = wabs


         do 10 j = 1,negrps

            ng_nescgp(k,j) = nescgp(j)
            g_enesc(k,j) = enesc(j)

 10      continue


      end if

      if( numMPItasks .eq. 1 ) return

c.... If I'm MPI task=0 and I know that I have no sends/receives to
c.... perform with other MPI tasks, then just return

      if( (MPIid .eq. 0) .AND. (srFlag .EQV. .FALSE.) )return


c.... Set up some buffers

      k = 11*OMP_GET_THREAD_NUM()

      ibuf(k+1) = npart
      ibuf(k+2) = nphtot
      ibuf(k+3) = nploss
      ibuf(k+4) = nlost
      ibuf(k+5) = nesc
      ibuf(k+6) = nrr
      ibuf(k+7) = ncen
      ibuf(k+8) = nabs
      ibuf(k+9) = nscat
      ibuf(k+10)= nsplt
      ibuf(k+11)= ntrac

      k = 7*OMP_GET_THREAD_NUM()

      rbuf(k+1) = etot
      rbuf(k+2) = epgain
      rbuf(k+3) = wlost
      rbuf(k+4) = wesc
      rbuf(k+5) = wrr
      rbuf(k+6) = wcen
      rbuf(k+7) = wabs
c.....Make sure all threads have written their part before proceeding
!$OMP BARRIER

c.....Send/recv ibuf.....

c.....Only one thread per MPI task performs communications
!$OMP MASTER
      if( MPIid .eq. 0 )then
         do 20 i = 1, numMPItasks-1
            CALL MPI_IRECV( jbuf(1,i), (numThreads*11), 
     &           MPI_INTEGER, MPI_ANY_SOURCE, 
     &           MPI_ANY_TAG, MPI_COMM_WORLD, req(i), ierr )
 20      continue

         CALL MPI_WAITALL( numMPItasks-1, req, statArray, ierr )

         do 30 i = 1, numMPItasks-1
            k = statArray(MPI_TAG,i) - 1
            DO 35 j = 1, numThreads
               kk = 11*(j-1)
               ng_npart(k+j)  = jbuf(kk+1,i)
               ng_nphtot(k+j) = jbuf(kk+2,i)
               ng_nploss(k+j) = jbuf(kk+3,i)
               ng_nlost(k+j)  = jbuf(kk+4,i)
               ng_nesc(k+j)   = jbuf(kk+5,i)
               ng_nrr(k+j)    = jbuf(kk+6,i)
               ng_ncen(k+j)   = jbuf(kk+7,i)
               ng_nabs(k+j)   = jbuf(kk+8,i)
               ng_nscat(k+j)  = jbuf(kk+9,i)
               ng_nsplt(k+j)  = jbuf(kk+10,i)
               ng_ntrac(k+j)  = jbuf(kk+11,i)
 35         continue
 30      continue
      end if


      if( MPIid .gt. 0 )then
         CALL MPI_SEND( ibuf, (numThreads*11), MPI_INTEGER, 0, 
     &        ithRun, MPI_COMM_WORLD, ierr )
      end if

      CALL MPI_BARRIER( MPI_COMM_WORLD, ierr )
!$OMP END MASTER


c.....Send/recv sbuf.....

c.....Only one thread per MPI task performs communications
!$OMP MASTER
      if( MPIid .eq. 0 )then
         do 40 i = 1, numMPItasks-1
            CALL MPI_IRECV( sbuf(1,i), 7*numThreads, 
     &           MPI_DOUBLE_PRECISION, 
     &           MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, req(i), 
     &           ierr )
 40      continue

         CALL MPI_WAITALL( numMPItasks-1, req, statArray, ierr )

         do 50 i = 1, numMPItasks-1

            k = statArray(MPI_TAG,i)-1  

            do 55 j = 1, numThreads
               kk = (j-1)*7
               g_etot(k+j)   = sbuf(kk+1,i)
               g_epgain(k+j) = sbuf(kk+2,i)
               g_wlost(k+j)  = sbuf(kk+3,i)
               g_wesc(k+j)   = sbuf(kk+4,i)
               g_wrr(k+j)    = sbuf(kk+5,i)
               g_wcen(k+j)   = sbuf(kk+6,i)
               g_wabs(k+j)   = sbuf(kk+7,i)

 55         continue
 50      continue
      end if

      if( MPIid .gt. 0 )then
         CALL MPI_SEND( rbuf, 7*numThreads, MPI_DOUBLE_PRECISION, 0,
     &        ithRun, MPI_COMM_WORLD, ierr )
      end if
      
      CALL MPI_BARRIER(MPI_COMM_WORLD,ierr)
!$OMP END MASTER


c.....Send/recv nescgp.....

      k = negrps*OMP_GET_THREAD_NUM()
      DO i = 1, negrps
         lescbuf(k+i) = nescgp(i)
      END DO
c.....Make sure all threads have written their part before proceeding
!$OMP BARRIER

c.....Only one thread per MPI task performs communications
!$OMP MASTER
      if( MPIid .eq. 0 )then
         do 60 i = 1, numMPItasks-1
            CALL MPI_IRECV( nescbuf(1,i), numThreads*negrps,
     &           MPI_INTEGER,  MPI_ANY_SOURCE, MPI_ANY_TAG, 
     &           MPI_COMM_WORLD, req(i), ierr )
 60      continue

         CALL MPI_WAITALL( numMPItasks-1, req, 
     &        statArray, ierr )

         do 70 i = 1, numMPItasks-1
            k = statArray(MPI_TAG,i) -1
            do 75 jj = 1, numThreads
               kk = (jj-1)*negrps
               do 80 j = 1,negrps
                  ng_nescgp(k+jj,j) = nescbuf(kk+j,i)
 80            continue
 75         continue
 70      continue
      end if

      if( MPIid .gt. 0 )then
         CALL MPI_SEND( lescbuf, numThreads*negrps, MPI_INTEGER, 0, 
     &        ithRun, MPI_COMM_WORLD, ierr )
      end if

      CALL MPI_BARRIER(MPI_COMM_WORLD,ierr)
!$OMP END MASTER


c.....Send/recv enesc....


      k = negrps*OMP_GET_THREAD_NUM()
      DO i = 1, negrps
         lenbuf(k+i) = enesc(i)
      END DO
c.....Make sure all threads have written their part before proceeding
!$OMP BARRIER

c.....Only one thread per MPI task performs communications
!$OMP MASTER
      if( MPIid .eq. 0 )then
         do 90 i = 1, numMPItasks-1
            CALL MPI_IRECV( enbuf(1,i), numThreads*negrps,
     &            MPI_DOUBLE_PRECISION,
     &           MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, req(i), 
     &           ierr )
 90      continue

         CALL MPI_WAITALL( numMPItasks-1, req, 
     &        statArray, ierr )

         do 100 i = 1, numMPItasks-1
            k = statArray(MPI_TAG,i)-1
            do 105 jj = 1, numThreads
               kk = (jj-1)*negrps
               do  110 j = 1,negrps
                  g_enesc(k+jj,j) = enbuf(kk+j,i)
 110           continue
 105        continue
 100     continue
      end if


      if( MPIid .gt. 0 )then
         CALL MPI_SEND( lenbuf, numThreads*negrps, MPI_DOUBLE_PRECISION,
     &         0, ithRun, MPI_COMM_WORLD, ierr )
      end if

      CALL MPI_BARRIER(MPI_COMM_WORLD,ierr)
!$OMP END MASTER


      return
      end
