LEVEL = ../../../..

#include $(LEVEL)/Makefile.config

LDFLAGS += -lm
LIBS    += -lm

include ../../Makefile.spec95

# The executable writes virtex.out, and RunSafely.sh appends the exit code to
# $(STDOUT_FILENAME). We need both.
LOCAL_OUTPUTS := vortex.out $(STDOUT_FILENAME)

ifeq ($(ARCH),Sparc)
  ## SPEC portability note for vortex says to use this flag on 64-bit machines
  CPPFLAGS += -D__RISC_64__
endif
ifeq ($(ARCH),Alpha)
  ## SPEC portability note for vortex says to use this flag on 64-bit machines
  CPPFLAGS += -D__RISC_64__
endif
ifeq ($(ARCH),IA64)
  ## SPEC portability note for vortex says to use this flag on 64-bit machines
  CPPFLAGS += -D__RISC_64__
endif
ifeq ($(ARCH),x86_64)
  ## SPEC portability note for vortex says to use this flag on 64-bit machines
  CPPFLAGS += -D__RISC_64__
endif

ifeq ($(ENDIAN), big)
  RUN_OPTIONS  = vortex.in.big
else
  RUN_OPTIONS  = vortex.in.little
endif
