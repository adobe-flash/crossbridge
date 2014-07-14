# MultiSource/minisat Makefile

LEVEL = ../../..
PROG = minisat
CPPFLAGS = -D NDEBUG
LDFLAGS = -lstdc++ -lm
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = -verbosity=0 $(PROJ_SRC_DIR)/small.cnf
else
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = -verbosity=0 $(PROJ_SRC_DIR)/long.cnf
else
RUN_OPTIONS = -verbosity=0 $(PROJ_SRC_DIR)/short.cnf
endif
endif
FP_TOLERANCE = 0.1

include ../../Makefile.multisrc

ifeq ($(TARGET_OS),Darwin)
SHLIBEXT := .dylib
endif

JIT_OPTS += -load libz$(SHLIBEXT)
