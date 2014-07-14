LEVEL = ../../../..
REQUIRES_EH_SUPPORT = 1
include $(LEVEL)/Makefile.config

SPEC_ROOT := $(SPEC2000_ROOT)

include $(PROJ_SRC_ROOT)/External/SPEC/Makefile.spec.config

CPPFLAGS += -DSPEC_CPU2000_LINUX_I386 -DSPEC_CPU2000_NEED_BOOL

# Not sure why this is needed. 
ifndef RUN_TYPE
ifdef SMALL_PROBLEM_SIZE
RUN_TYPE=test
else
ifdef LARGE_PROBLEM_SIZE
RUN_TYPE=ref
else
RUN_TYPE=train
endif
endif
endif

ifeq ($(RUN_TYPE),test)
RUN_OPTIONS     = test.pl
STDIN_FILENAME  := test.in
STDOUT_FILENAME := test.out
else
RUN_OPTIONS      = scrabbl.pl
STDIN_FILENAME  := scrabbl.in
STDOUT_FILENAME := scrabbl.out
endif

NT_SOURCES     = nt_perlmain.c win32.c win32sck.c win32threads.c perllib.c
UNIX_SOURCES   = unix_perlmain.c

ifeq ($(strip $(TARGET_OS)),Win32)
CPPFLAGS       += -Iwin32 -I. -DWIN32 -D_CONSOLE
LIBS           += advapi32.lib wsock32.lib
DO_NOT_COMPILE := $(UNIX_SOURCES)
else
DO_NOT_COMPILE := $(NT_SOURCES)
endif

Source := $(wildcard $(SPEC_BENCH_DIR)/src/*.c)
Source := $(filter-out $(foreach xyz,$(DO_NOT_COMPILE),%/$(xyz)),$(Source))

SPEC2K_DO_NOT_RECONFIGURE := 1
include $(LEVEL)/External/SPEC/Makefile.spec2000

ifeq ($(strip $(TARGET_OS)),Linux)
CPPFLAGS := $(filter-out -DSPEC_CPU2000,$(CPPFLAGS))
ifeq ($(ARCH),Alpha)
  CPPFLAGS += -DSPEC_CPU2000_LINUX_ALPHA
else
  CPPFLAGS += -DSPEC_CPU2000_LINUX_I386
endif
endif

ifeq ($(ENDIAN),little)
CPPFLAGS += -D__LITTLE_ENDIAN__
endif
