LEVEL = ../../../..
Source=libpbm1.c libpbm2.c libpbm3.c libpbm4.c libpbm5.c libpgm1.c libpgm2.c libppm1.c libppm2.c libppm3.c libppm4.c libppm5.c spec_image.c spec_jmemdst.c spec_jmemsrc.c spec_main.c rdppm.c wrppm.c rdgif.c wrgif.c rdtarga.c wrtarga.c rdbmp.c wrbmp.c jcapi.c jcparam.c jdatadst.c jcmaster.c jcmarker.c jcmainct.c jcprepct.c jccoefct.c jccolor.c jcsample.c jchuff.c jcdctmgr.c jfdctfst.c jfdctflt.c jfdctint.c jdapi.c jdatasrc.c jdmaster.c jdmarker.c jdmainct.c jdcoefct.c jdpostct.c jddctmgr.c jidctfst.c jidctflt.c jidctint.c jidctred.c jdhuff.c jdsample.c jdcolor.c jquant1.c jquant2.c jdmerge.c jcomapi.c jutils.c jerror.c jmemmgr.c jmemnobs.c
include ../../Makefile.spec95

ifeq ($(RUN_TYPE),test)
FILENAME  := specmun
else
FILENAME  := vigo
endif

STDOUT_FILENAME := $(FILENAME).out
RUN_OPTIONS := -image_file $(FILENAME).ppm -compression.quality 90 -compression.optimize_coding 0 -compression.smoothing_factor 90 -difference.image 1 -difference.x_stride 10 -difference.y_stride 10 -verbose 1 -GO.findoptcomp

# This #define is the perfectly logical way to get 132.ijpeg to #include errno.h
CPPFLAGS += -D__VMS

