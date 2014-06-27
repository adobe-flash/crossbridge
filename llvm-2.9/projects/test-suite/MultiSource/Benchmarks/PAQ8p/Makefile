LEVEL = ../../../

PROG = paq8p
CPPFLAGS += -DNOASM -DLLVM
LDFLAGS = -lstdc++ -lm
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = -1 $(PROJ_SRC_DIR)/small.in
else
RUN_OPTIONS = -4 $(PROJ_SRC_DIR)/file1.in
endif

include $(LEVEL)/MultiSource/Makefile.multisrc
