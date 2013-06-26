      subroutine mpi_abort ( comm, errorcode, ierror )

c*********************************************************************72
c
cc MPI_ABORT shuts down the processes in a given communicator.
c
c  Modified:
c
c    08 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Input, integer ERRORCODE, the error code to be returned.
c
c    Output, integer IERROR, an error code.
c
      implicit none

      integer comm
      integer errorcode
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_ABORT:'
      write ( *, '(a,i12)' ) 
     &  '  Shut down with error code = ', errorcode

      stop
      end
      subroutine mpi_allgather ( data1, nsend, sendtype, data2, 
     &  nrecv, recvtype, comm, ierror )

c*********************************************************************72
c
cc MPI_ALLGATHER gathers data from all the processes in a communicator.
c
c  Discussion:
c
c    This single processor version will copy values from DATA1 to DATA2.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer DATA1(NSEND), the data to be sent.
c
c    Input, integer NSEND, the number of data items to be sent.
c
c    Input, integer SENDTYPE, the MPI datatype of the data being sent.
c
c    Output, integer DATA2(NSEND*NUM_PROCS), the gathered data
c    that is received.
c
c    Input, integer NRECV, the number of data items to be received
c    from each process.
c
c    Input, integer RECVTYPE, the MPI datatype of the data being received.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer nsend

      integer comm
      integer data1(nsend)
      integer data2(nsend)
      integer ierror
      integer nrecv
      integer recvtype
      integer sendtype

      ierror = MPI_SUCCESS

      if ( sendtype .eq. mpi_double_precision ) then
        call mpi_copy_double_precision ( data1, data2, nsend, ierror )
      else if ( sendtype .eq. mpi_integer ) then
        call mpi_copy_integer ( data1, data2, nsend, ierror )
      else if ( sendtype .eq. mpi_real ) then
        call mpi_copy_real ( data1, data2, nsend, ierror )
      else
        ierror = MPI_FAILURE
      end if

      return
      end
      subroutine mpi_allgatherv ( data1, nsend, sendtype,
     &  data2, nrecv, ndispls, recvtype, comm, ierror )

c*********************************************************************72
c
cc MPI_ALLGATHERV gathers data from all the processes in a communicator.
c
c  Discussion:
c
c    This single processor version will copy values from DATA1 to DATA2.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer nsend

      integer comm
      integer data1(nsend)
      integer data2(nsend)
      integer ierror
      integer ndispls
      integer nrecv
      integer recvtype
      integer sendtype

      ierror = MPI_SUCCESS

      if ( sendtype .eq. mpi_double_precision ) then
        call mpi_copy_double_precision ( data1, data2, nsend, ierror )
      else if ( sendtype .eq. mpi_integer ) then
        call mpi_copy_integer ( data1, data2, nsend, ierror )
      else if ( sendtype .eq. mpi_real ) then
        call mpi_copy_real ( data1, data2, nsend, ierror )
      else
        ierror = MPI_FAILURE
      end if

      return
      end
      subroutine mpi_allreduce ( data1, data2, n, datatype,
     &  operation, comm, ierror )

c*********************************************************************72
c
cc MPI_ALLREDUCE carries out a reduction operation.
c
c  Discussion:
c
c    The reduction operations are MAXIMUM, MINIMUM, PRODUCT and SUM.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    07 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, DATATYPE DATA1(N), the data to be processed.
c
c    Output, DATATYPE DATA2, the value of the reduction operation.
c
c    Input, integer N, the number of items in DATA1.
c
c    Input, integer DATATYPE, indicates the datatype of DATA1 and DATA2.
c
c    Input, integer OPERATION, should have the value of one of the symbolic
c    constants MPI_MAX, MPI_MIN, MPI_PRODUCT or MPI_SUM.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer n

      integer comm
      integer data1(n)
      integer data2(n)
      integer datatype
      integer ierror
      integer operation

      ierror = MPI_SUCCESS

      if ( datatype .eq. mpi_double_precision ) then

        call mpi_reduce_double_precision ( 
     &    data1, data2, n, operation, ierror )

      else if ( datatype .eq. mpi_integer ) then

        call mpi_reduce_integer ( 
     &    data1, data2, n, operation, ierror )

      else if ( datatype .eq. mpi_real ) then

        call mpi_reduce_real ( 
     &    data1, data2, n, operation, ierror )

      else

        ierror = MPI_FAILURE

      end if

      return
      end
      subroutine mpi_barrier ( comm, ierror )

c*********************************************************************72
c
cc MPI_BARRIER forces processes within a communicator to wait together.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      return
      end
      subroutine mpi_bcast ( data, n, datatype, node, comm, ierror )

c*********************************************************************72
c
cc MPI_BCAST broadcasts data from one process to all others.
c
c  Discussion:
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    06 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, datatype DATA(N), the data to be broadcast.
c
c    Input, integer N, the number of items of data.
c
c    Input, integer DATATYPE, the MPI code for the datatype of the data.
c
c    Input, integer NODE, the rank of the sending process within the
c    given communicator.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )
      integer node

      ierror = MPI_SUCCESS

      return
      end
      subroutine mpi_bsend ( data, n, datatype, iproc, itag,
     &  comm, ierror )

c*********************************************************************72
c
cc MPI_BSEND sends data from one process to another, using buffering.
c
c  Discussion:
c
c    Warn against sending message to self, since no data copy is done.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    06 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, datatype DATA(N), the data to be sent.
c
c    Input, integer N, the number of data items to send.
c
c    Input, integer DATAYTPE, the MPI code for the datatype.
c
c    Input, integer IPROC, the rank of the process within the communicator
c    that is to receive the message.
c
c    Input, integer ITAG, a tag for the message.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer iproc
      integer itag
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_BSEND - Error!'
      write ( *, '(a)' )  '  Should not send message to self.'

      return
      end
      subroutine mpi_cart_create ( comm, ndims, dims, periods,
     &  reorder, comm_cart, ierror )

c*********************************************************************72
c
cc MPI_CART_CREATE creates a communicator for a Cartesian topology.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer COMM_CART, the new MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer ndims

      integer comm
      integer comm_cart
      integer dims(*)
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )
      logical periods(*)
      logical reorder

      ierror = MPI_SUCCESS

      return
      end
      subroutine mpi_cart_get ( comm, ndims, dims, periods,
     &  coords, ierror )

c*********************************************************************72
c
cc MPI_CART_GET returns the "Cartesian coordinates" of the calling process.
c
c  Discussion:
c
c    Set all coordinates to 0.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer ndims

      integer comm
      integer coords(*)
      integer dims(*)
      integer i
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )
      logical periods(*)

      ierror = MPI_SUCCESS

      do i = 1, ndims
        coords(i) = 0
      end do

      return
      end
      subroutine mpi_cart_shift ( comm, idir, idisp, isource, 
     &  idest, ierror )

c*********************************************************************72
c
cc MPI_CART_SHIFT finds the destination and source for Cartesian shifts.
c
c  Discussion:
c
c    Set ISOURCE = IDEST = SELF = 0.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer idest
      integer idir
      integer idisp
      integer ierror
      integer isource
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS
      isource = 0
      idest = 0

      return
      end
      subroutine mpi_comm_dup ( comm, comm_out, ierror )

c*********************************************************************72
c
cc MPI_COMM_DUP duplicates a communicator.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer COMM_OUT, the new MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer comm_out
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS
      comm_out = comm

      return
      end
      subroutine mpi_comm_free ( comm, ierror )

c*********************************************************************72
c
cc MPI_COMM_FREE "frees" a communicator.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      return
      end
      subroutine mpi_comm_rank ( comm, me, ierror )

c*********************************************************************72
c
cc MPI_COMM_RANK reports the rank of the calling process.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer ierror
      integer me
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS
      me = 0

      return
      end
      subroutine mpi_comm_size ( comm, nprocs, ierror )

c*********************************************************************72
c
cc MPI_COMM_SIZE reports the number of processes in a communicator.
c
c  Discussion:
c
c    The routine simply returns NPROCS = 1.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )
      integer nprocs

      ierror = MPI_SUCCESS
      nprocs = 1

      return
      end
      subroutine mpi_comm_split ( comm, icolor, ikey, comm_new,
     &  ierror )

c*********************************************************************72
c
cc MPI_COMM_SPLIT splits up a communicator based on a key.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Input, integer COMM_NEW, the new MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer comm
      integer comm_new
      integer icolor
      integer ierror
      integer ikey
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      return
      end
      subroutine mpi_copy_double_precision ( data1, data2, n, ierror )

c*********************************************************************72
c
cc MPI_COPY_DOUBLE copies a double precision vector.
c
c  Discussion:
c
c    This routine is not part of the MPI standard.  However, it is
c    needed by other routines which do emulate standard MPI routines.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, double precision DATA1(N), the data to be copied.
c
c    Output, double precision DATA2(N), the copied data.
c
c    Input, integer N, the number of items of data.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      double precision data1(n)
      double precision data2(n)
      integer i
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      do i = 1, n
        data2(i) = data1(i)
      end do

      return
      end
      subroutine mpi_copy_integer ( data1, data2, n, ierror )

c*********************************************************************72
c
cc MPI_COPY_INTEGER copies an integer vector.
c
c  Discussion:
c
c    This routine is not part of the MPI standard.  However, it is
c    needed by other routines which do emulate standard MPI routines.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer DATA1(N), the data to be copied.
c
c    Output, integer DATA2(N), the copied data.
c
c    Input, integer N, the number of items of data.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer data1(n)
      integer data2(n)
      integer i
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      do i = 1, n
        data2(i) = data1(i)
      end do

      return
      end
      subroutine mpi_copy_real ( data1, data2, n, ierror )

c*********************************************************************72
c
cc MPI_COPY_REAL copies a real vector.
c
c  Discussion:
c
c    This routine is not part of the MPI standard.  However, it is
c    needed by other routines which do emulate standard MPI routines.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, real DATA1(N), the data to be copied.
c
c    Output, real DATA2(N), the copied data.
c
c    Input, integer N, the number of items of data.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      real data1(n)
      real data2(n)
      integer i
      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      do i = 1, n
        data2(i) = data1(i)
      end do

      return
      end
      subroutine mpi_finalize ( ierror )

c*********************************************************************72
c
cc MPI_FINALIZE shuts down the MPI library.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      return
      end
      subroutine mpi_get_count ( istatus, datatype, icount, ierror )

c*********************************************************************72
c
cc MPI_GET_COUNT reports the actual number of items transmitted.
c
c  Discussion:
c
c    Warn against querying message from self, since no data copy is done.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer datatype
      integer icount
      integer ierror
      integer istatus
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_GET_COUNT - Error!'
      write ( *, '(a)' ) '  Should not query message from self.'

      return
      end
      subroutine mpi_init ( ierror )

c*********************************************************************72
c
cc MPI_INIT initializes the MPI library.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer ierror
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_SUCCESS

      return
      end
      subroutine mpi_irecv ( data, n, datatype, iproc, itag,
     &  comm, irequest, ierror )

c*********************************************************************72
c
cc MPI_IRECV receives data from another process.
c
c  Discussion:
c
c    Warn against receiving message from self, since no data copy is done.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer iproc
      integer irequest
      integer itag
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_IRECV - Error!'
      write ( *, '(a)' ) '  Should not recv message from self.'

      return
      end
      subroutine mpi_isend ( data, n, datatype, iproc, itag,
     &  comm, request, ierror )

c*********************************************************************72
c
cc MPI_ISEND sends data from one process to another using nonblocking transmission.
c
c  Discussion:
c
c    Warn against sending message to self, since no data copy is done.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    15 August 2008
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, datatype DATA(N), the data to be sent.
c
c    Input, integer N, the number of data items to send.
c
c    Input, integer DATAYTPE, the MPI code for the datatype.
c
c    Input, integer IPROC, the rank of the process within the communicator
c    that is to receive the message.
c
c    Input, integer ITAG, a tag for the message.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer REQUEST, a handle.  To determine if the data has been received
c    yet, call MPI_Test or MPI_Wait, including the value of REQUEST.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer iproc
      integer itag
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )
      integer request

      request = 0
      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_ISEND - Error!'
      write ( *, '(a)' )  '  Should not send message to self.'

      return
      end
      subroutine mpi_recv ( data, n, datatype, iproc, itag,
     &  comm, istatus, ierror )

c*********************************************************************72
c
cc MPI_RECV receives data from another process within a communicator.
c
c  Discussion:
c
c    Warn against receiving message from self, since no data copy is done.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer iproc
      integer istatus
      integer itag
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_RECV - Error!'
      write ( *, '(a)' ) '  Should not recv message from self.'

      return
      end
      subroutine mpi_reduce ( data1, data2, n, datatype, operation,
     &  receiver, comm, ierror )

c*********************************************************************72
c
cc MPI_REDUCE carries out a reduction operation.
c
c  Discussion:
c
c    The reduction operations are sum, maximum, minimum, product.
c
c    The first two arguments must not overlap or share memory in any way.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, DATATYPE DATA1(N), the data to be processed.
c
c    Output (to RECEIVER only), DATATYPE DATA2, the value of the
c    reduction operation.
c
c    Input, integer N, the number of items in DATA1.
c
c    Input, integer DATATYPE, indicates the datatype of DATA1 and DATA2.
c
c    Input, integer OPERATION, should have the value of one of the symbolic
c    constants MPI_MAX, MPI_MIN, MPI_PRODUCT or MPI_SUM.
c
c    Input, integer RECEIVER, the the process that is to receive the
c    result.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer n

      integer comm
      integer data1(n)
      integer data2
      integer datatype
      integer ierror
      integer operation
      integer receiver

      ierror = MPI_SUCCESS

      if ( datatype .eq. mpi_double_precision ) then

        call mpi_reduce_double_precision ( 
     &    data1, data2, n, operation, ierror )

      else if ( datatype .eq. mpi_integer ) then

        call mpi_reduce_integer ( 
     &    data1, data2, n, operation, ierror )

      else if ( datatype .eq. mpi_real ) then

        call mpi_reduce_real ( 
     &    data1, data2, n, operation, ierror )

      else

        ierror = MPI_FAILURE

      end if

      return
      end
      subroutine mpi_reduce_double_precision ( 
     &  data1, data2, n, operation, ierror )

c*********************************************************************72
c
cc MPI_REDUCE_DOUBLE_PRECISION carries out a reduction operation on double precision values.
c
c  Discussion:
c
c    The reduction operations are sum, maximum, minimum, product.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, double precision DATA1(N), the data to be processed.
c
c    Output, double precision DATA2, the value of the reduction operation.
c
c    Input, integer N, the number of items in DATA1.
c
c    Input, integer OPERATION, should have the value of one of the symbolic
c    constants MPI_MAX, MPI_MIN, MPI_PRODUCT or MPI_SUM.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer n

      double precision data1(n)
      double precision data2
      integer i
      integer ierror
      integer operation


      ierror = MPI_SUCCESS

      if ( operation .eq. mpi_max ) then

        data2 = data1(1)
        do i = 2, n
          data2 = max ( data2, data1(i) )
        end do

      else if ( operation .eq. mpi_min ) then

        data2 = data1(1)
        do i = 2, n
          data2 = min ( data2, data1(i) )
        end do

      else if ( operation .eq. mpi_product ) then

        data2 = data1(1)
        do i = 2, n
          data2 = data2 * data1(i)
        end do

      else if ( operation .eq. mpi_sum ) then

        data2 = data1(1)
        do i = 2, n
          data2 = data2 + data1(i)
        end do

      else

        ierror = MPI_FAILURE

      end if

      return
      end
      subroutine mpi_reduce_integer ( 
     &  data1, data2, n, operation, ierror )

c*********************************************************************72
c
cc MPI_REDUCE_INTEGER carries out a reduction operation on integers.
c
c  Discussion:
c
c    The reduction operations are sum, maximum, minimum, product.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, integer DATA1(N), the data to be processed.
c
c    Output, integer DATA2, the value of the reduction operation.
c
c    Input, integer N, the number of items in DATA1.
c
c    Input, integer OPERATION, should have the value of one of the symbolic
c    constants MPI_MAX, MPI_MIN, MPI_PRODUCT or MPI_SUM.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer n

      integer data1(n)
      integer data2
      integer i
      integer ierror
      integer operation

      ierror = MPI_SUCCESS

      if ( operation .eq. mpi_max ) then

        data2 = data1(1)
        do i = 2, n
          data2 = max ( data2, data1(i) )
        end do

      else if ( operation .eq. mpi_min ) then

        data2 = data1(1)
        do i = 2, n
          data2 = min ( data2, data1(i) )
        end do

      else if ( operation .eq. mpi_product ) then

        data2 = data1(1)
        do i = 2, n
          data2 = data2 * data1(i)
        end do

      else if ( operation .eq. mpi_sum ) then

        data2 = data1(1)
        do i = 2, n
          data2 = data2 + data1(i)
        end do

      else

        ierror = MPI_FAILURE

      end if

      return
      end
      subroutine mpi_reduce_real ( 
     &  data1, data2, n, operation, ierror )

c*********************************************************************72
c
cc MPI_REDUCE_REAL carries out a reduction operation on reals.
c
c  Discussion:
c
c    The reduction operations are sum, maximum, minimum, product.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, real DATA1(N), the data to be processed.
c
c    Output, real DATA2, the value of the reduction operation.
c
c    Input, integer N, the number of items in DATA1.
c
c    Input, integer OPERATION, should have the value of one of the symbolic
c    constants MPI_MAX, MPI_MIN, MPI_PRODUCT or MPI_SUM.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer n

      real data1(n)
      real data2
      integer i
      integer ierror
      integer operation

      ierror = MPI_SUCCESS

      if ( operation .eq. mpi_max ) then

        data2 = data1(1)
        do i = 2, n
          data2 = max ( data2, data1(i) )
        end do

      else if ( operation .eq. mpi_min ) then

        data2 = data1(1)
        do i = 2, n
          data2 = min ( data2, data1(i) )
        end do

      else if ( operation .eq. mpi_product ) then

        data2 = data1(1)
        do i = 2, n
          data2 = data2 * data1(i)
        end do

      else if ( operation .eq. mpi_sum ) then

        data2 = data1(1)
        do i = 2, n
          data2 = data2 + data1(i)
        end do

      else

        ierror = MPI_FAILURE

      end if

      return
      end
      subroutine mpi_reduce_scatter ( data1, data2, n, datatype,
     &  operation, comm, ierror )

c*********************************************************************72
c
cc MPI_REDUCE_SCATTER collects a message of the same length from each process.
c
c  Discussion:
c
c    Copy values from DATA1 to DATA2.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, DATATYPE DATA1(N), the data to be processed.
c
c    Output, DATATYPE DATA2, the value of the reduction operation.
c
c    Input, integer N, the number of items in DATA1.
c
c    Input, integer DATATYPE, indicates the datatype of DATA1 and DATA2.
c
c    Input, integer OPERATION, should have the value of one of the symbolic
c    constants MPI_MAX, MPI_MIN, MPI_PRODUCT or MPI_SUM.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      include "mpi_stubs_f77.h"

      integer n

      integer comm
      integer data1(n)
      integer data2(n)
      integer datatype
      integer ierror
      integer operation

      ierror = MPI_SUCCESS

      if ( datatype .eq. mpi_double_precision ) then
        call mpi_copy_double_precision ( data1, data2, n, ierror )
      else if ( datatype .eq. mpi_integer ) then
        call mpi_copy_integer ( data1, data2, n, ierror )
      else if ( datatype .eq. mpi_real ) then
        call mpi_copy_real ( data1, data2, n, ierror )
      else
        ierror = MPI_FAILURE
      end if

      return
      end
      subroutine mpi_rsend ( data, n, datatype, iproc, itag,
     &  comm, ierror )

c*********************************************************************72
c
cc MPI_RSEND "ready sends" data from one process to another.
c
c  Discussion:
c
c    Warn against sending message to self, since no data copy is done.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, datatype DATA(N), the data to be sent.
c
c    Input, integer N, the number of data items to send.
c
c    Input, integer DATAYTPE, the MPI code for the datatype.
c
c    Input, integer IPROC, the rank of the process within the communicator
c    that is to receive the message.
c
c    Input, integer ITAG, a tag for the message.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer iproc
      integer itag
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_RSEND - Error!'
      write ( *, '(a)' ) '  Should not send message to self.'

      return
      end
      subroutine mpi_send ( data, n, datatype, iproc, itag,
     &  comm, ierror )

c*********************************************************************72
c
cc MPI_SEND sends data from one process to another.
c
c  Discussion:
c
c    Warn against sending message to self, since no data copy is done.
c
c    The data to be transferred can be integer, real, or double precision.
c    In this routine, it is declared and documented as INTEGER type, 
c    but using the other types should generally not cause a problem.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Input, datatype DATA(N), the data to be sent.
c
c    Input, integer N, the number of data items to send.
c
c    Input, integer DATAYTPE, the MPI code for the datatype.
c
c    Input, integer IPROC, the rank of the process within the communicator
c    that is to receive the message.
c
c    Input, integer ITAG, a tag for the message.
c
c    Input, integer COMM, the MPI communicator.
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer n

      integer comm
      integer data(n)
      integer datatype
      integer ierror
      integer iproc
      integer itag
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_SEND - Error!'
      write ( *, '(a)' )  '  Should not send message to self.'

      return
      end
      subroutine mpi_wait ( irequest, istatus, ierror )

c*********************************************************************72
c
cc MPI_WAIT waits for an I/O request to complete.
c
c  Discussion:
c
c    Warn against waiting on message from self, since no data copy is done.
c
c  Modified:
c
c    04 October 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer ierror
      integer irequest
      integer istatus
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_WAIT - Error!'
      write ( *, '(a)' ) '  Should not wait on message from self.'

      return
      end
      subroutine mpi_waitall ( icount, irequest, istatus, ierror )

c*********************************************************************72
c
cc MPI_WAITALL waits until all I/O requests have completed.
c
c  Discussion:
c
c    Warn against waiting on message from self, since no data copy is done.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer icount
      integer ierror
      integer irequest
      integer istatus
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_WAITALL - Error!'
      write ( *, '(a)' ) '  Should not wait on message from self.'

      return
      end
      subroutine mpi_waitany ( icount, array_of_requests, index, 
     &  istatus, ierror )

c*********************************************************************72
c
cc MPI_WAITANY waits until one I/O requests has completed.
c
c  Discussion:
c
c    Warn against waiting on message from self, since no data copy is done.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, integer IERROR, is nonzero if an error occurred.
c
      implicit none

      integer array_of_requests(*)
      integer icount
      integer ierror
      integer index
      integer istatus
      integer MPI_FAILURE
      parameter ( MPI_FAILURE = 1 )
      integer MPI_SUCCESS
      parameter ( MPI_SUCCESS = 0 )

      ierror = MPI_FAILURE

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'MPI_WAITANY - Error!'
      write ( *, '(a)' ) '  Should not wait on message from self.'

      return
      end
      function mpi_wtick ( )

c*********************************************************************72
c
cc MPI_WTICK returns the time between clock ticks.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, double precision MPI_WTICK, the time, in seconds, between
c    successive clock ticks.
c
      implicit none
      
      double precision mpi_wtick
      
      mpi_wtick = 1.0D+00
      
      return
      end
      function mpi_wtime ( )

c*********************************************************************72
c
cc MPI_WTIME returns the elapsed wall clock time.
c
c  Discussion:
c
c    While you might be able to get a wall clock reading if you
c    happen to know the peculiarities of the extra routines in your
c    own FORTRAN77 compiler, there is no standard way to do this,
c    and I am tired of trying to make up for this.
c
c  Modified:
c
c    05 February 2007
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    William Gropp, Ewing Lusk, Anthony Skjellum,
c    Using MPI: Portable Parallel Programming with the
c    Message-Passing Interface,
c    Second Edition,
c    MIT Press, 1999,
c    ISBN: 0262571323.
c
c  Parameters:
c
c    Output, double precision MPI_WTIME, the elapsed wall clock time.
c
      implicit none

      double precision fake_time
      double precision mpi_wtime
      
      save fake_time

      data fake_time / 0.0D+00 /

      mpi_wtime = fake_time

      fake_time = fake_time + 1.0D+00
      
      return
      end
      subroutine timestamp ( )

c*********************************************************************72
c
cc TIMESTAMP prints out the current YMDHMS date as a timestamp.
c
c  Discussion:
c
c    This FORTRAN77 version is made available for cases where the
c    FORTRAN90 version cannot be used.
c
c  Modified:
c
c    12 January 2007
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    None
c
      implicit none

      character * ( 8 ) ampm
      integer d
      character * ( 8 ) date
      integer h
      integer m
      integer mm
      character * ( 9 ) month(12)
      integer n
      integer s
      character * ( 10 ) time
      integer y

      save month

      data month /
     &  'January  ', 'February ', 'March    ', 'April    ', 
     &  'May      ', 'June     ', 'July     ', 'August   ', 
     &  'September', 'October  ', 'November ', 'December ' /

      call date_and_time ( date, time )

      read ( date, '(i4,i2,i2)' ) y, m, d
      read ( time, '(i2,i2,i2,1x,i3)' ) h, n, s, mm

      if ( h .lt. 12 ) then
        ampm = 'AM'
      else if ( h .eq. 12 ) then
        if ( n .eq. 0 .and. s .eq. 0 ) then
          ampm = 'Noon'
        else
          ampm = 'PM'
        end if
      else
        h = h - 12
        if ( h .lt. 12 ) then
          ampm = 'PM'
        else if ( h .eq. 12 ) then
          if ( n .eq. 0 .and. s .eq. 0 ) then
            ampm = 'Midnight'
          else
            ampm = 'AM'
          end if
        end if
      end if

      write ( *, 
     &  '(i2,1x,a,1x,i4,2x,i2,a1,i2.2,a1,i2.2,a1,i3.3,1x,a)' ) 
     &  d, month(m), y, h, ':', n, ':', s, '.', mm, ampm

      return
      end

cccccccccccccccccc fake openmp crud
      function omp_get_num_threads()
      integer omp_get_num_threads
      omp_get_num_threads = 1
      return
      end
      
      function omp_get_thread_num()
      integer omp_get_thread_num
      omp_get_thread_num = 0
      return
      end
      
