LEVEL = ../../../

Source = lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c\
	 lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c\
	 lundump.c lvm.c lzio.c lauxlib.c lbaselib.c ldblib.c liolib.c lmathlib.c\
	 loslib.c ltablib.c lstrlib.c loadlib.c linit.c lua.c

LDFLAGS += -lm
CPPFLAGS += -DLUA_USE_POSIX

PROG = lua
RUN_OPTIONS = alltests.lua "\<in"
HASH_PROGRAM_OUTPUT = 1

include $(LEVEL)/Makefile.config

include ../../Makefile.multisrc
DIFFPROG := $(PROGDIR)/DiffOutput.sh "diff "

# generate input files
$(PROJ_SRC_DIR)/lua.c: .input_generated

.input_generated:
	if [ ! -f ./generate_inputs.sh ]; \
	then \
		cp -f $(PROJ_SRC_DIR)/generate_inputs.sh . ; \
		cp -f -r $(PROJ_SRC_DIR)/test . ; \
		cp -f -r $(PROJ_SRC_DIR)/bench . ; \
		cp -f -r $(PROJ_SRC_DIR)/input . ; \
		cp -f $(PROJ_SRC_DIR)/*.lua . ; \
	fi
	./generate_inputs.sh

