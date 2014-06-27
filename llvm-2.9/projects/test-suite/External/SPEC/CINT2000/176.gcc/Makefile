LEVEL = ../../../..

HASH_PROGRAM_OUTPUT = 1

include ../../Makefile.spec2000

ifeq ($(RUN_TYPE),test)
RUN_OPTIONS     = cccp.i -o - -quiet
STDOUT_FILENAME = cccp.s
endif
ifeq ($(RUN_TYPE),train)
RUN_OPTIONS     = cp-decl.i -o - -quiet
STDOUT_FILENAME = cp-decl.s
endif
ifeq ($(RUN_TYPE),ref)
RUN_OPTIONS     = 200.i -o - -quiet
STDOUT_FILENAME = 200.s
endif

ifeq ($(ENDIAN),big)
  CPPFLAGS += -DHOST_WORDS_BIG_ENDIAN
endif

ifeq ($(ARCH),Sparc)
  ## SPEC portability note for GCC says to use these flags and cross fingers:
  CPPFLAGS += -DSPEC_CPU2000_LP64
endif
