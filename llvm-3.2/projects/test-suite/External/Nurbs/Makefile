LEVEL = ../..

include $(LEVEL)/Makefile.config

PROG = nurbs
SourceDir := $(NURBS_ROOT)

CPPFLAGS =
LDFLAGS = -lstdc++
LIBS += -lstdc++

ifeq ($(ARCH),x86_64)
  CPPFLAGS += -DDWORD_IS_ULL
endif

RUN_OPTIONS = /k all timed /t 500 /vsteps 192 /usteps 192 /vcp 20 /ucp 20

include $(LEVEL)/MultiSource/Makefile.multisrc
