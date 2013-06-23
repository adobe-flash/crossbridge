LEVEL = ../../../..

PROG     = pcompress2
CPPFLAGS = -DVERSION='"1.00"' -DCOMPDATE="\"today\"" -DCFLAGS='""' -DHOSTNAME="\"thishost\"" 
#LDFLAGS  = -lm
ifdef LARGE_PROBLEM_SIZE
INPUT_FILE := run.in
else
INPUT_FILE := test.in
endif
RUN_OPTIONS = $(PROJ_OBJ_DIR)/$(INPUT_FILE)

# The pcompress2 benchmark wants to write an output file adjacent to the input
# file. In order to avoid writing back into the source directory, we copy the
# input file into the output directory.
GeneratedTestInputs := copy-input-file

include $(LEVEL)/MultiSource/Makefile.multisrc

copy-input-file:
	if [ ! $(PROJ_SRC_DIR) = $(PROJ_OBJ_DIR) ]; \
	then \
	  cp  $(PROJ_SRC_DIR)/$(INPUT_FILE) $(PROJ_OBJ_DIR)/$(INPUT_FILE); \
	fi
.PHONY: copy-input-files
