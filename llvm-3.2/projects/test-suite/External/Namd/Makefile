LEVEL = ../..

include $(LEVEL)/Makefile.config

PROG = namd
SourceDir := $(NAMD_ROOT)

CFLAGS += -Wno-deprecated
CPPFLAGS += -Wno-deprecated
LIBS += -lm -lstdc++
LDFLAGS = -lm -lstdc++

#
# Expand the runtime limit as this is a very intensive benchmark
#
RUNTIMELIMIT := 1000

RUN_OPTIONS = --iterations 1 --input $(NAMD_ROOT)/apoa1.input
include $(LEVEL)/MultiSource/Makefile.multisrc
