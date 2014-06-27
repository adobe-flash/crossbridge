      subroutine copyglob       !Copy global info to private arrays.   

      include 'params.inc'
      include 'geomz.inc'
      include 'times.inc'
      include 'mpif.h'

      real*8 tstart, tstop, dt(maxMPItasks), tdiff
      integer MPIid, ierr, statArray(MPI_STATUS_SIZE, maxMPItasks)
      integer req(maxMPItasks)

      CALL MPI_COMM_SIZE( MPI_COMM_WORLD, numMPItasks, ierr )
      CALL MPI_COMM_RANK( MPI_COMM_WORLD, MPIid, ierr )
      
!      CALL second(tstart) 

      do 901 j = 1, nzmax
         do 900 k = 1, 5
            rr(j,k)    = g_rr(j,k)
            zz(j,k)    = g_zz(j,k)
            itype(j,k) = ng_itype(j,k)
            bom(j,k)   = g_bom(j,k)
            sqm(j,k)   = g_sqm(j,k)
 900     continue 
         ximp(j) = g_ximp(j)
         mid(j)  = ng_mid(j)
         volcl(j)= g_volcl(j)
 901  continue
      do 902 j = 1, 5
         incr(j) = ng_incr(j)
 902  continue
      do 904 j = 1, nrzmax
         ximpl(j)= g_ximpl(j)
 904  continue
!      CALL second(tstop)
      tdiff = tstop - tstart

      if( numMPItasks .eq. 1 )then
         copyGlobTime(1) = tdiff
         return
      end if

      if( MPIid .eq. 0 )then
         copyGlobTime(1) = tdiff
         if( numMPItasks .gt. 1 )then
            do i = 1, numMPItasks-1
               CALL MPI_IRECV( dt(i), 1, MPI_DOUBLE_PRECISION, 
     &              MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, req(i), 
     &              ierr )
            end do
            CALL MPI_WAITALL( numMPItasks-1, req, 
     &           statArray, ierr )
            do i = 1, numMPItasks-1
               copyGlobTime(statArray(MPI_TAG,i)) = dt(i)
            end do
         end if
      end if

      if( MPIid .gt. 0 )then
         CALL MPI_SEND( tdiff, 1, MPI_DOUBLE_PRECISION, 
     &        0, MPIid+1, MPI_COMM_WORLD, ierr )
      end if

      if( numMPItasks .gt. 1 )then
         CALL MPI_BARRIER( MPI_COMM_WORLD, ierr )
      end if

      return
      END

      BLOCK DATA TINIT
      include 'times.inc'

      data copyGlobTime /maxMPItasks*0.0/
      END
