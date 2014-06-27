
      subroutine second(t)

      include 'mpif.h'

      real*8 t

      t = MPI_WTIME()

      return
      end
