      
      subroutine rdopac (ilib,illib)
c***********************************************************************
c Read material opacities as a function of temperature and density
c
c Modified 3/15/01 bmb: For file system scalability, substituted
c MPI_Bcast instead of having each task read the input file individually.
c Also disabled read/write of binary opacity file for ASCI Purple
c benchmark purposes. It is not portable.  Therefore, the ilib input
c file parameter must be 1.  The illib parameter is ignored.
c***********************************************************************
      
c     matl code   matl
c     1             h
c     2             sio2
c     3             dt
c     4             c

      implicit double precision (a-h,o-z)
      
      include 'mpif.h'

      DOUBLE PRECISION x(14,32,13,4), t(32,2), d(14)
      integer ierr, myid

      common /opactab/ x, t, d


      CALL MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)

      if (ilib .eq. 1) then

c     TEXT read of the opacity file.
c     Task 0 is only task to actually perform I/O.  It then
c     broadcasts the input file via MPI_BCAST to all other tasks

        if (myid .eq. 0) then
          open (4, file='opac.txt')
          do 701 l = 1,4
            do 701 k = 1,13
              do 701 j = 1,32
                do 701 i = 1,14
                  read (4,704) x(i,j,k,l)
 701      continue
 704      format (e20.4)
          close (4)
        endif

        CALL MPI_BARRIER(MPI_COMM_WORLD, ierr)
        CALL MPI_BCAST(x, 23296, MPI_DOUBLE_PRECISION, 0,
     +     MPI_COMM_WORLD, ierr) 

      endif


      if (ilib .ne. 1) then

        if (myid .eq. 0) then
        print *, " "
        print *, "ERROR: rdopac routine:" 
        print *, "Read/write of binary opacity file feature is not"
        print *, "enabled for ASCI Purple benchmark purposes. Please " 
        print *, "check your input file and make sure that the ilib"
        print *, "parameter = 1." 
        print *, " "
        endif

        CALL MPI_FINALIZE(ierr)
        stop

      endif

c
c     if(illib .ne. 0 .and. ilib .ne. 1) then
c--------------------do a BINARY write of the opacity file
c           open (15, file=  'opac.bin',
c    |           access='sequential',
c    |           form=  'unformatted',
c    |           status='new')
c     write (15) x
c           do 88804 l=1,4
c              do 88804 k=1,13
c                 write(15) 
c    .                 ((x(i,j,k,l),i=1,14),j=1,32)
c88804          continue
c              close (15)
c              stop
c           endif
c           
c        else
c--------------------do a BINARY read of the opacity file
c           open (15, file=  'opac.bin',
c    |           access='sequential',
c    |           form=  'unformatted',
c    |           status='old')
c     read (15) x
c           do 88805 l=1,4
c              do 88805 k=1,13
c                 read(15) 
c    .                 ((x(i,j,k,l),i=1,14),j=1,32)
c88805          continue
c              close(15)
c           endif
            
  
      return
      end


      BLOCK DATA RDOINIT

      implicit double precision (a-h,o-z)
      
      DOUBLE PRECISION x(14,32,13,4), t(32,2), d(14)
      
      common /opactab/ x, t, d
      
c     temperatures for tabulated opacities (64 total):
c     first  set of 32 for material 1 or 3 (h or d),
c     second set of 32 for material 2 or 4 (sio2 or c).
      
      data t / 79.d0,99.d0,125.d0,157.d0,198.d0,250.d0,315.d0,397.d0,
     + 500.d0,630.d0,974.d0,
     +     1000.d0,1260.d0,1587.d0,2000.d0,2520.d0,3175.d0,4000.d0,
     +  5040.d0,6350.d0,8000.d0,
     +     10079.d0,12699.d0,16000.d0,20159.d0,25398.d0,32000.d0,
     + 40317.d0,50797.d0,64000.d0,
     +     80635.d0,101594.d0,
     +     20.d0,25.d0,31.d0,39.d0,50.d0,63.d0,79.d0,99.d0,125.d0,
     + 157.d0,198.d0,250.d0,315.d0,397.d0,
     +     500.d0,630.d0,974.d0,1000.d0,1260.d0,1587.d0,2000.d0,2520.d0,
     + 3175.d0,4000.d0,5040.d0,
     +     6350.d0,8000.d0,10079.d0,12699.d0,16000.d0,20159.d0,25398.d0/

c     densities for tabulated opacities (14 total)
      
      data d / 3.162d-4,1.000d-3,3.162d-3,1.000d-2,3.162d-2,1.000d-1,
     .     3.162d-1,1.000d+0,3.162d+0,1.000d+1,3.162d+1,1.000d+2,
     .     3.162d+2,1.000d+3/
            
      END
