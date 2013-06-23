LEVEL = ../../../..

PROG     = consumer-typeset
CPPFLAGS = -DOS_UNIX=1 -DOS_DOS=0 -DOS_MAC=0 -DDB_FIX=0 -DUSE_STAT=1 -DSAFE_DFT=0 -DCOLLATE=1 -DLIB_DIR=\"lout.lib\" -DFONT_DIR=\"font\" -DMAPS_DIR=\"maps\" -DINCL_DIR=\"include\" -DDATA_DIR=\"data\" -DHYPH_DIR=\"hyph\" -DLOCALE_DIR=\"locale\" -DCHAR_IN=1 -DCHAR_OUT=0 -DLOCALE_ON=1 -DASSERT_ON=1 -DDEBUG_ON=0  -DPDF_COMPRESSION=0
LDFLAGS  = -lm
RUN_OPTIONS = -x -I $(PROJ_SRC_DIR)/data/include -D $(PROJ_SRC_DIR)/data/data -F $(PROJ_SRC_DIR)/data/font -C $(PROJ_SRC_DIR)/data/maps -H $(PROJ_OBJ_DIR)/data/hyph $(PROJ_SRC_DIR)/large.lout

# The consumer-typeset benchmark wants to write output files (caches derived
# from the inputs, I think) into the same dir as the hyphenation files. In order
# to avoid writing back into the source directory, we copy the hyphenation files
# into the output directory.
GeneratedTestInputs := copy-hyph-files

include $(LEVEL)/MultiSource/Makefile.multisrc

copy-hyph-files:
	if [ ! $(PROJ_SRC_DIR) = $(PROJ_OBJ_DIR) ]; \
	then \
	  mkdir -p  $(PROJ_OBJ_DIR)/data/hyph/; \
	  cp $(PROJ_SRC_DIR)/data/hyph/* $(PROJ_OBJ_DIR)/data/hyph/; \
	fi
.PHONY: copy-hyph-files
