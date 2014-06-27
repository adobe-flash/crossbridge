LEVEL = ../../../..

include $(LEVEL)/Makefile.config

# This test uses the deprecated re_comp functions.
ifdef HAVE_RE_COMP
PROG = archie
endif

include $(LEVEL)/MultiSource/Makefile.multisrc

ifeq ($(TARGET_OS),SunOS)
LDFLAGS += -lsocket -lnsl
endif
