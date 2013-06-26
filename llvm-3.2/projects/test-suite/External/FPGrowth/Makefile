LEVEL = ../..

include $(LEVEL)/Makefile.config

PROG = fpgrowth
SourceDir := $(FPGROWTH_ROOT)

STDIN_FILENAME := $(PROJ_SRC_DIR)/input
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = $(FPGROWTH_ROOT)/pumsb_star.dat
else
RUN_OPTIONS = $(FPGROWTH_ROOT)/mushroom.dat
endif
include $(LEVEL)/MultiSource/Makefile.multisrc
