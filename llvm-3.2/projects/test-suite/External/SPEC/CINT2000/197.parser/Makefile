LEVEL = ../../../..
RUN_OPTIONS     = 2.1.dict -batch
STDIN_FILENAME  = $(RUN_TYPE).in
STDOUT_FILENAME = $(RUN_TYPE).out
CPPFLAGS =

include $(LEVEL)/Makefile.config

ifeq ($(TARGET_OS),Darwin)
CPPFLAGS += -D_ANSI_SOURCE=1
endif

include ../../Makefile.spec2000
