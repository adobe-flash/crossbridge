##===- Makefile.spec ---------------------------------------*- Makefile -*-===##
#
# This makefile is a template for building SPEC as an external test. It is
# included by Makefile.spec2006, Makefile.spec2000, and Makefile.spec95.
#
##===----------------------------------------------------------------------===##

include $(LEVEL)/MultiSource/Makefile.multisrc

CPPFLAGS += -I $(SPEC_BENCH_DIR)/src/
SPEC_SANDBOX := $(PROGDIR)/External/SPEC/Sandbox.sh

# Information about testing the program...
REF_IN_DIR  := $(SPEC_BENCH_DIR)/data/$(RUN_TYPE)/input/
REF_OUT_DIR := $(SPEC_BENCH_DIR)/data/$(RUN_TYPE)/output/
LOCAL_OUTPUTS = $(notdir $(wildcard $(REF_OUT_DIR)/*))

# Sometimes a test will output its program name or some other gunk that
# we don't want to diff against. Tests can override this definition to
# provide a filter for the output files.
SPEC_OUTPUT_FILE_FILTER = cat


# Specify how to generate output from the SPEC programs.  Basically we just run
# the program in a sandbox (a special directory we create), then we cat all of
# the outputs together.

ifndef USE_REFERENCE_OUTPUT

$(PROGRAMS_TO_TEST:%=Output/%.out-nat): \
Output/%.out-nat: Output/%.native
	$(SPEC_SANDBOX) nat-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$*.native $(RUN_OPTIONS)
	-(cd Output/nat-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/nat-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif
ifdef UPDATE_REFERENCE_OUTPUT
ifeq ($(REFERENCE_OUTPUT_KEY),)
	cp $@ $(PROJ_SRC_DIR)/$*.reference_output
else
	if [ ! -f $(PROJ_SRC_DIR)/$*.reference_output ]; then \
	  echo "error: no normal reference output!"; \
	  exit 1; \
	elif (! diff -q $@ $(PROJ_SRC_DIR)/$*.reference_output); then \
	  cp $@ $(PROJ_SRC_DIR)/$*.reference_output.$(REFERENCE_OUTPUT_KEY); \
	else \
	  echo "no need to update $(REFERENCE_OUTPUT_KEY) reference," \
	       "matches normal reference!"; \
	fi
endif
endif

else

# Otherwise, pick the best reference output based on
# 'progamname.reference_output'.
#
# Note that this rule needs to be in both Makefile.programs and Makefile.spec.
Output/%.out-nat: Output/.dir
	-if [ -f "$(PROJ_SRC_DIR)/$*.reference_output.$(REFERENCE_OUTPUT_KEY)" ]; then \
	  cp $(PROJ_SRC_DIR)/$*.reference_output.$(REFERENCE_OUTPUT_KEY) $@; \
	elif [ -f "$(PROJ_SRC_DIR)/$*.reference_output" ]; then \
	  cp $(PROJ_SRC_DIR)/$*.reference_output $@; \
	else \
	  printf "WARNING: %s: %s\n" "NO REFERENCE OUTPUT (using default)" "$(PROJ_SRC_DIR)/$*.reference_output" > $@; \
	  cp $(PROJ_SRC_ROOT)/default.reference_output $@; \
	  cat $@; \
	fi

endif

$(PROGRAMS_TO_TEST:%=Output/%.out-simple): \
Output/%.out-simple: Output/%.simple
	$(SPEC_SANDBOX) simple-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$*.simple $(RUN_OPTIONS)
	-(cd Output/simple-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/simple-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-lli): \
Output/%.out-lli: Output/%.llvm.bc $(LLI)
	$(SPEC_SANDBOX) lli-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  $(LLI) $(LLI_OPTS) ../$*.llvm.bc $(RUN_OPTIONS)
	-(cd Output/lli-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/lli-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-jit): \
Output/%.out-jit: Output/%.llvm.bc $(LLI)
	$(SPEC_SANDBOX) jit-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  $(LLI) $(JIT_OPTS) ../$*.llvm.bc $(RUN_OPTIONS)
	-(cd Output/jit-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/jit-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-jit-beta): \
Output/%.out-jit-beta: Output/%.llvm.bc $(LLI)
	$(SPEC_SANDBOX) jit-beta-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  $(LLI) $(LLCBETAOPTION) $(JIT_OPTS) ../$*.llvm.bc $(RUN_OPTIONS)
	-(cd Output/jit-beta-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/jit-beta-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-llc): \
Output/%.out-llc: Output/%.llc
	$(SPEC_SANDBOX) llc-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$*.llc $(RUN_OPTIONS)
	-(cd Output/llc-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/llc-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-llc-beta): \
Output/%.out-llc-beta: Output/%.llc-beta
	$(SPEC_SANDBOX) llc-beta-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$*.llc-beta $(RUN_OPTIONS)
	-(cd Output/llc-beta-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/llc-beta-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-opt-beta): \
Output/%.out-opt-beta: Output/%.opt-beta
	$(SPEC_SANDBOX) opt-beta-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$*.opt-beta $(RUN_OPTIONS)
	-(cd Output/opt-beta-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/opt-beta-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.out-cbe): \
Output/%.out-cbe: Output/%.cbe
	$(SPEC_SANDBOX) cbe-$(RUN_TYPE) $@ $(REF_IN_DIR) \
             $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$*.cbe $(RUN_OPTIONS)
	-(cd Output/cbe-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/cbe-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

# The RunSafely.sh script puts an "exit <retval>" line at the end of
# the program's output. We have to make bugpoint do the same thing
# or else it will get false positives when it diff's the reference
# output with the program's output.
BUGPOINT_OPTIONS += -append-exit-code

# If a tolerance is set, pass it off to bugpoint.
#
# Note: this uses '$(if ...)' instead of ifeq so that FP_TOLERANCE and
# FP_ABSTOLERANCE can be "recursively expanded". The purpose of this is
# to allow the indivdiaul benchmark Makefiles to set FP_TOLERANCE
# after including this file, which they may need to do if they wish to
# have it depend on the value of RUN_TYPE.
BUGPOINT_OPTIONS += $(if $(FP_TOLERANCE),-rel-tolerance $(FP_TOLERANCE),)
BUGPOINT_OPTIONS += $(if $(FP_ABSTOLERANCE),-abs-tolerance $(FP_ABSTOLERANCE),)


# Give bugpoint information about LDFLAGS to pass down to the actual link stage
# of the program.
BUGPOINT_OPTIONS += $(LDFLAGS:%=-Xlinker=%) $(EXTRA_OPTIONS:%=-Xlinker=%)


# Specify stdin, reference output, and command line options for the program...
BUGPOINT_OPTIONS += -input=$(STDIN_FILENAME) -output=../$*.out-nat
BUGPOINT_OPTIONS += -timeout=$(RUNTIMELIMIT)
BUGPOINT_OPTIONS += --tool-args $(LLCFLAGS)
BUGPOINT_ARGS += --args -- $(RUN_OPTIONS)

# Rules to bugpoint the opt, llc, or lli commands...
$(PROGRAMS_TO_TEST:%=Output/%.bugpoint-opt): \
Output/%.bugpoint-opt: Output/%.noopt-llvm.bc $(LBUGPOINT) \
                         Output/%.out-nat
	$(SPEC_SANDBOX) bugpoint-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	    env PWD=$(CURDIR) $(LBUGPOINT) -llc-safe ../$*.noopt-llvm.bc -std-compile-opts $(OPTPASSES) \
	    $(BUGPOINT_OPTIONS) $(BUGPOINT_ARGS)
	@echo "===> Leaving Output/bugpoint-$(RUN_TYPE)"

$(PROGRAMS_TO_TEST:%=Output/%.bugpoint-llc): \
Output/%.bugpoint-llc: Output/%.llvm.bc $(LBUGPOINT) Output/%.out-nat
	$(SPEC_SANDBOX) bugpoint-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	    env PWD=$(CURDIR) $(LBUGPOINT) ../$*.llvm.bc -run-llc $(BUGPOINT_OPTIONS) $(BUGPOINT_ARGS)
	@echo "===> Leaving Output/bugpoint-$(RUN_TYPE)"

$(PROGRAMS_TO_TEST:%=Output/%.bugpoint-llc-beta): \
Output/%.bugpoint-llc-beta: Output/%.llvm.bc $(LBUGPOINT) Output/%.out-nat
	$(SPEC_SANDBOX) bugpoint-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	    env PWD=$(CURDIR) $(LBUGPOINT) ../$*.llvm.bc -run-llc $(BUGPOINT_OPTIONS) \
	    $(LLCBETAOPTION) $(BUGPOINT_ARGS)
	@echo "===> Leaving Output/bugpoint-$(RUN_TYPE)"

$(PROGRAMS_TO_TEST:%=Output/%.bugpoint-jit): \
Output/%.bugpoint-jit: Output/%.llvm.bc $(LBUGPOINT) Output/%.out-nat
	$(SPEC_SANDBOX) bugpoint-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	    env PWD=$(CURDIR) $(LBUGPOINT) ../$*.llvm.bc -safe-run-llc -run-jit $(BUGPOINT_OPTIONS) $(BUGPOINT_ARGS)
	@echo "===> Leaving Output/bugpoint-$(RUN_TYPE)"

$(PROGRAMS_TO_TEST:%=Output/%.bugpoint-jit-beta): \
Output/%.bugpoint-jit-beta: Output/%.llvm.bc $(LBUGPOINT) Output/%.out-nat
	$(SPEC_SANDBOX) bugpoint-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	    env PWD=$(CURDIR) $(LBUGPOINT) ../$*.llvm.bc -run-jit $(BUGPOINT_OPTIONS) \
	    $(LLCBETAOPTION) $(BUGPOINT_ARGS)
	@echo "===> Leaving Output/bugpoint-$(RUN_TYPE)"

$(PROGRAMS_TO_TEST:%=Output/%.bugpoint-cbe): \
Output/%.bugpoint-cbe: Output/%.llvm.bc $(LBUGPOINT) Output/%.out-nat
	$(SPEC_SANDBOX) bugpoint-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	    env PWD=$(CURDIR) $(LBUGPOINT) ../$*.llvm.bc -cbe-bug $(BUGPOINT_OPTIONS) \
	    $(BUGPOINT_ARGS)
	@echo "===> Leaving Output/bugpoint-$(RUN_TYPE)"


LIBPROFILESO = $(LLVMLIBCURRENTSOURCE)/libprofile_rt.so

# rules for PGO
$(PROGRAMS_TO_TEST_PROFGEN:%=Output/%.out-pgo): \
Output/%.out-pgo: Output/%
	$(VERB) $(RM) -f Output/$*.prof-data.tmp
	LLVMPROF_OUTPUT="../$*.prof-data.tmp" \
	$(SPEC_SANDBOX) profile-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	     $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$* $(RUN_OPTIONS)
	-(cd Output/profile-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
          $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/profile-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST_ALLUSE:%=Output/%.out-pgo): \
Output/%.out-pgo: Output/%
	$(VERB) $(RM) -f Output/$*.prof-data.tmp
	$(SPEC_SANDBOX) pgo-$(RUN_TYPE) $@ $(REF_IN_DIR) \
	     $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) \
                  ../$* $(RUN_OPTIONS)
	-(cd Output/pgo-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
          $(SPEC_OUTPUT_FILE_FILTER) > $@
	-cp Output/pgo-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST:%=Output/%.prof): \
Output/%.prof: Output/%.llvm-prof.bc Output/%.out-nat $(LIBPROFILESO)
	@rm -f $@
	$(SPEC_SANDBOX) profile-$(RUN_TYPE) Output/$*.out-prof $(REF_IN_DIR) \
	  $(RUNSAFELY) $(STDIN_FILENAME) $(STDOUT_FILENAME) $(LLI) $(JIT_OPTS)\
            -fake-argv0 '../$*.llvm.bc' -load $(LIBPROFILESO) ../../$< -llvmprof-output ../../$@ $(RUN_OPTIONS)
	-(cd Output/profile-$(RUN_TYPE); cat $(LOCAL_OUTPUTS)) | \
	  $(SPEC_OUTPUT_FILE_FILTER) > Output/$*.out-prof
	-cp Output/profile-$(RUN_TYPE)/$(STDOUT_FILENAME).time $@.time
	-cp Output/profile-$(RUN_TYPE)/llvmprof.out $@
	@cmp -s Output/$*.out-prof Output/$*.out-nat || \
		printf "***\n***\n*** WARNING: Output of profiled program (Output/$*.out-prof)\n*** doesn't match the output of the native program (Output/$*.out-nat)!\n***\n***\n";
