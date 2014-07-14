! -*- fortran -*-
!
! Copyright 1998-2001, University of Notre Dame.
! Authors: Jeffrey M. Squyres, Arun Rodrigues, and Brian Barrett with
!          Kinis L. Meyer, M. D. McNally, and Andrew Lumsdaine
! 
! This file is part of the Notre Dame LAM implementation of MPI.
! 
! You should have received a copy of the License Agreement for the Notre
! Dame LAM implementation of MPI along with the software; see the file
! LICENSE.  If not, contact Office of Research, University of Notre
! Dame, Notre Dame, IN 46556.
! 
! Redistribution and use in source and binary forms, with or without
! modification, are permitted subject to the conditions specified in the
! LICENSE file.
! 
! THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
! IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
! WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
! DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
! INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
! (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
! SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
! HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
! STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
! IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
! POSSIBILITY OF SUCH DAMAGE.
! 
! Additional copyrights may follow.
! 
!
!  $Id: mpif.h.in,v 1.2 2000/09/20 05:03:21 jsquyres Exp $
!
!	Function:	- LAM/MPI F77 header file
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
! Do ***not*** copy this file to the directory where your Fortran 
! fortran application is compiled unless it is absolutely necessary!  Most
! modern Fortran compilers now support the -I command line flag, which
! tells the compiler where to find .h files (specifically, this one).  For
! example:
!
!      unix% mpif77 foo.f -o foo -I$LAMHOME/include
!
! will probably do the trick (assuming that you have set LAMHOME 
! properly).
!
! That being said, LAM's "mpif77" wrapper compiler should
! automatically include the -I option for you.  The following command
! should be equivalent to the command listed above:
!
!      unix% mpif77 foo.f -o foo
!
! You should not copy this file to your local directory because it is
! possible that this file will be changed between versions of LAM/MPI.
! Indeed, this mpif.h is incompatible with the mpif.f of other 
! implementations of MPI.  Using this mpif.h with other implementations 
! of MPI, or with other versions of LAM/MPI will result in undefined
! behavior (to include incorrect results, segmentation faults, 
! unexplainable "hanging" in your application, etc.).  Always use the
! -I command line option instead (or let mpif77 do it for you).
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!
! LAM version
! This file is generated from configure; do not edit it manually.
!
       integer LAM_MAJOR_VERSION, LAM_MINOR_VERSION
       integer LAM_RELEASE_VERSION
       integer LAM_ALPHA_VERSION, LAM_BETA_VERSION
       parameter (LAM_MAJOR_VERSION=6)
       parameter (LAM_MINOR_VERSION=5)
       parameter (LAM_RELEASE_VERSION=6)
       parameter (LAM_ALPHA_VERSION=0)
       parameter (LAM_BETA_VERSION=0)
!
! MPI version
!
       integer MPI_VERSION, MPI_SUBVERSION

       parameter (MPI_VERSION=1)
       parameter (MPI_SUBVERSION=2)
!
! misc. constants
!
       integer MPI_SUCCESS, MPI_ANY_SOURCE, MPI_ANY_TAG
       integer MPI_PROC_NULL, MPI_MAX_PROCESSOR_NAME
       integer MPI_MAX_ERROR_STRING, MPI_UNDEFINED
       integer MPI_CART, MPI_GRAPH, MPI_KEYVAL_INVALID
       integer MPI_STATUS_SIZE, MPI_SOURCE, MPI_TAG, MPI_ERROR
       integer MPI_TAG_UB, MPI_HOST, MPI_IO, MPI_WTIME_IS_GLOBAL
       integer MPI_UNIVERSE_SIZE, MPI_APPNUM, MPI_WIN_BASE
       integer MPI_WIN_SIZE, MPI_WIN_DISP_UNIT, MPI_BSEND_OVERHEAD
       integer MPI_MAX_INFO_KEY, MPI_MAX_INFO_VAL
       integer MPI_MAX_PORT_NAME, MPI_MAX_OBJECT_NAME
       integer MPI_ORDER_C, MPI_ORDER_FORTRAN
       integer MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_CYCLIC
       integer MPI_DISTRIBUTE_NONE, MPI_DISTRIBUTE_DFLT_DARG

       parameter (MPI_SUCCESS=0)
       parameter (MPI_ANY_SOURCE=-1)
       parameter (MPI_ANY_TAG=-1)
       parameter (MPI_PROC_NULL=-2)
       parameter (MPI_MAX_PROCESSOR_NAME=255)
       parameter (MPI_MAX_ERROR_STRING=255)
       parameter (MPI_UNDEFINED=-32766)
       parameter (MPI_CART=1)
       parameter (MPI_GRAPH=2)
       parameter (MPI_KEYVAL_INVALID=-1)
       parameter (MPI_STATUS_SIZE=4)
       parameter (MPI_SOURCE=1)
       parameter (MPI_TAG=2)
       parameter (MPI_ERROR=3)
       parameter (MPI_TAG_UB=0)
       parameter (MPI_HOST=1)
       parameter (MPI_IO=2)
       parameter (MPI_WTIME_IS_GLOBAL=3)
       parameter (MPI_UNIVERSE_SIZE=4)
       parameter (MPI_APPNUM=5)
       parameter (MPI_WIN_BASE=6)
       parameter (MPI_WIN_SIZE=7)
       parameter (MPI_WIN_DISP_UNIT=8)
       parameter (MPI_BSEND_OVERHEAD=40)
       parameter (MPI_MAX_INFO_KEY=35)
       parameter (MPI_MAX_INFO_VAL=255)
       parameter (MPI_MAX_PORT_NAME=35)
       parameter (MPI_MAX_OBJECT_NAME=63)
       parameter (MPI_ORDER_C=0)
       parameter (MPI_ORDER_FORTRAN=1)
       parameter (MPI_DISTRIBUTE_BLOCK=0)
       parameter (MPI_DISTRIBUTE_CYCLIC=1)
       parameter (MPI_DISTRIBUTE_NONE=2)
       parameter (MPI_DISTRIBUTE_DFLT_DARG=-1)
!
! global variables
!
       double complex MPI_BOTTOM, MPI_ARGV_NULL
       double complex MPI_ARGVS_NULL, MPI_ERRCODES_IGNORE
       double complex MPI_STATUS_IGNORE, MPI_STATUSES_IGNORE
       common/mpi_bottom/MPI_BOTTOM
       common/mpi_argv_null/MPI_ARGV_NULL
       common/mpi_argvs_null/MPI_ARGVS_NULL
       common/mpi_errcodes_ignore/MPI_ERRCODES_IGNORE
       common/mpi_status_ignore/MPI_STATUS_IGNORE
       common/mpi_statuses_ignore/MPI_STATUSES_IGNORE
!
! NULL "handles" (indices)
!
       integer MPI_GROUP_NULL, MPI_COMM_NULL, MPI_DATATYPE_NULL
       integer MPI_REQUEST_NULL, MPI_OP_NULL, MPI_ERRHANDLER_NULL
       integer MPI_INFO_NULL

       parameter (MPI_GROUP_NULL=-1)
       parameter (MPI_COMM_NULL=-1)
       parameter (MPI_DATATYPE_NULL=-1)
       parameter (MPI_REQUEST_NULL=-1)
       parameter (MPI_OP_NULL=-1)
       parameter (MPI_ERRHANDLER_NULL=-1)
       parameter (MPI_INFO_NULL=-1)
!
! MPI_Init_thread constants
!
       integer MPI_THREAD_SINGLE, MPI_THREAD_FUNNELED
       integer MPI_THREAD_SERIALIZED, MPI_THREAD_MULTIPLE

       parameter (MPI_THREAD_SINGLE=0)
       parameter (MPI_THREAD_FUNNELED=1)
       parameter (MPI_THREAD_SERIALIZED=2)
       parameter (MPI_THREAD_MULTIPLE=3)
!
! error classes
!
       integer MPI_ERR_BUFFER, MPI_ERR_COUNT, MPI_ERR_TYPE
       integer MPI_ERR_TAG, MPI_ERR_COMM, MPI_ERR_RANK
       integer MPI_ERR_REQUEST, MPI_ERR_ROOT, MPI_ERR_GROUP
       integer MPI_ERR_OP, MPI_ERR_TOPOLOGY, MPI_ERR_DIMS
       integer MPI_ERR_ARG, MPI_ERR_UNKNOWN, MPI_ERR_TRUNCATE
       integer MPI_ERR_OTHER, MPI_ERR_INTERN, MPI_ERR_IN_STATUS
       integer MPI_ERR_PENDING, MPI_ERR_SYSRESOURCE
       integer MPI_ERR_LOCALDEAD, MPI_ERR_REMOTEDEAD
       integer MPI_ERR_VALUE, MPI_ERR_FLAGS, MPI_ERR_SERVICE
       integer MPI_ERR_NAME, MPI_ERR_SPAWN, MPI_ERR_KEYVAL
       integer MPI_ERR_INFO_NOKEY, MPI_ERR_WIN
       integer MPI_ERR_EPOCH, MPI_ERR_TYPENOTSUP
       integer MPI_ERR_INFO_KEY, MPI_ERR_INFO_VALUE
       integer MPI_ERR_NO_MEM, MPI_ERR_BASE
       integer MPI_ERR_LASTCODE

       parameter (MPI_ERR_BUFFER=1)
       parameter (MPI_ERR_COUNT=2)
       parameter (MPI_ERR_TYPE=3)
       parameter (MPI_ERR_TAG=4)
       parameter (MPI_ERR_COMM=5)
       parameter (MPI_ERR_RANK=6)
       parameter (MPI_ERR_REQUEST=7)
       parameter (MPI_ERR_ROOT=8)
       parameter (MPI_ERR_GROUP=9)
       parameter (MPI_ERR_OP=10)
       parameter (MPI_ERR_TOPOLOGY=11)
       parameter (MPI_ERR_DIMS=12)
       parameter (MPI_ERR_ARG=13)
       parameter (MPI_ERR_UNKNOWN=14)
       parameter (MPI_ERR_TRUNCATE=15)
       parameter (MPI_ERR_OTHER=16)
       parameter (MPI_ERR_INTERN=17)
       parameter (MPI_ERR_IN_STATUS=18)
       parameter (MPI_ERR_PENDING=19)
       parameter (MPI_ERR_SYSRESOURCE=20)
       parameter (MPI_ERR_LOCALDEAD=21)
       parameter (MPI_ERR_REMOTEDEAD=22)
       parameter (MPI_ERR_VALUE=23)
       parameter (MPI_ERR_FLAGS=24)
       parameter (MPI_ERR_SERVICE=25)
       parameter (MPI_ERR_NAME=26)
       parameter (MPI_ERR_SPAWN=27)
       parameter (MPI_ERR_KEYVAL=28)
       parameter (MPI_ERR_INFO_NOKEY=29)
       parameter (MPI_ERR_WIN=30)
       parameter (MPI_ERR_EPOCH=31)
       parameter (MPI_ERR_TYPENOTSUP=32)
       parameter (MPI_ERR_INFO_KEY=33)
       parameter (MPI_ERR_INFO_VALUE=34)
       parameter (MPI_ERR_NO_MEM=35)
       parameter (MPI_ERR_BASE=36)
       parameter (MPI_ERR_LASTCODE=37)
!
! comparison results
!
       integer MPI_IDENT, MPI_CONGRUENT, MPI_SIMILAR, MPI_UNEQUAL

       parameter (MPI_IDENT=1)
       parameter (MPI_CONGRUENT=2)
       parameter (MPI_SIMILAR=3)
       parameter (MPI_UNEQUAL=4)
!
! lookup table indices
!
       integer MPI_COMM_WORLD, MPI_COMM_SELF
       integer MPI_GROUP_EMPTY
       integer MPI_ERRORS_ARE_FATAL, MPI_ERRORS_RETURN

       parameter (MPI_COMM_WORLD=0)
       parameter (MPI_COMM_SELF=1)
       parameter (MPI_GROUP_EMPTY=2)
       parameter (MPI_ERRORS_ARE_FATAL=3)
       parameter (MPI_ERRORS_RETURN=4)

       integer MPI_INTEGER, MPI_REAL, MPI_DOUBLE_PRECISION
       integer MPI_COMPLEX, MPI_LOGICAL, MPI_CHARACTER
       integer MPI_BYTE, MPI_PACKED, MPI_UB, MPI_LB, MPI_2REAL
       integer MPI_2DOUBLE_PRECISION, MPI_2INTEGER
       integer MPI_DOUBLE_COMPLEX

       parameter (MPI_BYTE=5)
       parameter (MPI_PACKED=6)
       parameter (MPI_UB=7)
       parameter (MPI_LB=8)
       parameter (MPI_CHARACTER=9)
       parameter (MPI_LOGICAL=10)
       parameter (MPI_INTEGER=11)
       parameter (MPI_REAL=12)
       parameter (MPI_DOUBLE_PRECISION=13)
       parameter (MPI_COMPLEX=14)
       parameter (MPI_DOUBLE_COMPLEX=15)
       parameter (MPI_2REAL=16)
       parameter (MPI_2DOUBLE_PRECISION=17)
       parameter (MPI_2INTEGER=18)

       integer MPI_MAX, MPI_MIN, MPI_SUM, MPI_PROD, MPI_LAND
       integer MPI_BAND, MPI_LOR, MPI_BOR, MPI_LXOR, MPI_BXOR
       integer MPI_MAXLOC, MPI_MINLOC, MPI_REPLACE

       parameter (MPI_MAX=19)
       parameter (MPI_MIN=20)
       parameter (MPI_SUM=21)
       parameter (MPI_PROD=22)
       parameter (MPI_LAND=23)
       parameter (MPI_BAND=24)
       parameter (MPI_LOR=25)
       parameter (MPI_BOR=26)
       parameter (MPI_LXOR=27)
       parameter (MPI_BXOR=28)
       parameter (MPI_MAXLOC=29)
       parameter (MPI_MINLOC=30)
       parameter (MPI_REPLACE=31)
!
! attribute functions
!
       external MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN
       external MPI_COMM_NULL_COPY_FN, MPI_COMM_NULL_DELETE_FN
       external MPI_TYPE_NULL_COPY_FN, MPI_TYPE_NULL_DELETE_FN
       external MPI_WIN_NULL_COPY_FN, MPI_WIN_NULL_DELETE_FN
       external MPI_DUP_FN, MPI_COMM_DUP_FN
       external MPI_TYPE_DUP_FN, MPI_WIN_DUP_FN
!
! double precision functions
!
      double precision MPI_WTIME, MPI_WTICK, PMPI_WTIME, PMPI_WTICK
      external MPI_WTIME, MPI_WTICK, PMPI_WTIME, PMPI_WTICK
