LEVEL = ../../../..
RUN_OPTIONS := 31415926
LDFLAGS += -lstdc++
HASH_PROGRAM_OUTPUT = 1

ifeq ($(ARCH),x86)
ifdef USE_REFERENCE_OUTPUT
EXEC_XFAILS = cast
endif
endif

include $(LEVEL)/SingleSource/Makefile.singlesrc
