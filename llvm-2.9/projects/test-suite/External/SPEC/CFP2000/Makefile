LEVEL = ../../..

include $(LEVEL)/Makefile.config

## C Programs
PARALLEL_DIRS :=   \
	177.mesa   \
	179.art    \
	183.equake \
	188.ammp

ifdef USE_F95
PARALLEL_DIRS += \
        168.wupwise \
        171.swim    \
        172.mgrid   \
        173.applu   \
        178.galgel  \
	187.facerec \
	189.lucas   \
	191.fma3d   \
        200.sixtrack \
        301.apsi
else
ifdef USE_F2C

# These are f90 and hence cannot be converted with f2c:
# 178.galgel, 187.facerec, 189.lucas, 191.fma3d
#
# These are broken with f2c: output does not compile natively
# 200.sixtrack, 301.apsi
PARALLEL_DIRS += \
	168.wupwise \
	171.swim    \
	172.mgrid   \
	173.applu
endif
endif   # End USE_F95

include $(LEVEL)/Makefile.programs
