LEVEL = ../../../..

Source = modules.f90 ab.f90 arhbt.f90 arhim.f90 bifg21.f90 bifgel.f90          \
            bifoag.f90 cyklap.f90 eigQR.f90 farhim.f90 funht.f90 funns.f90     \
            galgel.f90 grsyst.f90 htxyl.f90 htxynl.f90 newtap.f90 newtlap.f90  \
            nsxyar.f90 nsxyl.f90 nsxynl.f90 pollin.f90 polnel.f90 pro.f90      \
            syshtL.f90 syshtN.f90 sysnsL.f90 sysnsN.f90 tempbt.f90 tempo.f90   \
            tminit.f90 tnelgo.f90 tsubo.f90 vxrigid.f90 vyfree.f90 xconduc.f90 \
            yadiab.f90 lapak.f90

FP_ABSTOLERANCE=2.0e-8
FP_TOLERANCE=0.01

STDIN_FILENAME  = galgel.in
STDOUT_FILENAME = galgel.out
NAGFORTRAN_FLAGS = -fixed -kind=byte -dcfuns -dusty
# -I $(SPEC_BENCH_DIR)/src


include ../../Makefile.spec2000
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN
CPPFLAGS += -DINT64='long long'

ab.c arhbt.c arhim.c bifg21.c bifgel.c          \
            bifoag.c cyklap.c eigQR.c farhim.c funht.c funns.c     \
            galgel.c grsyst.c htxyl.c htxynl.c newtap.c newtlap.c  \
            nsxyar.c nsxyl.c nsxynl.c pollin.c polnel.c pro.c      \
            syshtL.c syshtN.c sysnsL.c sysnsN.c tempbt.c tempo.c   \
            tminit.c tnelgo.c tsubo.c vxrigid.c vyfree.c xconduc.c \
            yadiab.c lapak.c: modules.c
