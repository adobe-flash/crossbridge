LEVEL = ../../../..
#STDOUT_FILENAME := input.random.out

include ../../Makefile.spec2000

# pick the big or little endian data file based on the host.
ifeq ($(ENDIAN),big)
  RUN_OPTIONS     = bendian.raw
else
  RUN_OPTIONS     = lendian.raw
endif
