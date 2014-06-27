LEVEL = ../../../

Source = sqlite3.c shell.c

PROG = sqlite3
CPPFLAGS += -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=0 -DHAVE_FDATASYNC=0 -DHAVE_USLEEP=0 -DHAVE_LOCALTIME_R=0 -DHAVE_GMTIME_R=0 -DHAVE_READLINE=0 -I. -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION=1
LDFLAGS =
STDIN_FILENAME = $(PROJ_SRC_DIR)/commands
RUN_OPTIONS = :memory:
FP_TOLERANCE = 1.0e-9

include $(LEVEL)/Makefile.config

include ../../Makefile.multisrc

# the input files are generated from a tcl script
# it needs to be generated before sqlite is run
sqlite3.c: $(PROJ_SRC_DIR)/test15.sql 

ifdef SMALL_PROBLEM_SIZE
$(PROJ_SRC_DIR)/test15.sql: $(PROJ_SRC_DIR)/smalltest.tcl
	$(TCLSH) $(PROJ_SRC_DIR)/smalltest.tcl
else
$(PROJ_SRC_DIR)/test15.sql: $(PROJ_SRC_DIR)/speedtest.tcl
	$(TCLSH) $(PROJ_SRC_DIR)/speedtest.tcl
endif

clean::
	rm -f test*.sql
