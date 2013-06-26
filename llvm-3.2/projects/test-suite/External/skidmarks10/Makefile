LEVEL = ../..
include $(LEVEL)/Makefile.config

PROG = skidmarks

DISABLE_JIT = 1

SourceDir := $(SKIDMARKS10_ROOT)/source
RUN_OPTIONS = 0 $(SKIDMARKS10_ROOT)/golden

CPPFLAGS += -I$(SKIDMARKS10_ROOT)/source

# skidmarks requires sse3 on x86
ifneq (,$(findstring x86,$(ARCH)))
CPPFLAGS += -msse3
endif

# TODO: We may want to set more flags than the default run here.

include $(LEVEL)/MultiSource/Makefile.multisrc
