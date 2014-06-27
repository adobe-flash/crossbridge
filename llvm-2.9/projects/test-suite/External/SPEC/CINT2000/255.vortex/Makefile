LEVEL = ../../../..
#STDOUT_FILENAME := input.random.out

include ../../Makefile.spec2000

# pick the big or little endian data file based on the host.
ifeq ($(ENDIAN),big)
  RUN_OPTIONS     = bendian.raw
else
  RUN_OPTIONS     = lendian.raw
endif

ifeq ($(ARCH),Sparc)
  ## SPEC portability note for vortex says to use this flag on 64-bit machines
  CPPFLAGS += -DSPEC_CPU2000_LP64
endif
