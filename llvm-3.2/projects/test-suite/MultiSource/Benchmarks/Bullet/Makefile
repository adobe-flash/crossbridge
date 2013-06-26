LEVEL = ../../../
PROG = bullet
CPPFLAGS += -I$(PROJ_SRC_DIR)/include -DNO_TIME
LDFLAGS = -lstdc++ -lm

# This test can take more than the default 500s timeout on Thumb1
RUNTIMELIMIT:=1000

include $(LEVEL)/Makefile.config

include ../../Makefile.multisrc
