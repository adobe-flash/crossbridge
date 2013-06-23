##===- TEST.profile.Makefile ------------------------------*- Makefile -*--===##
#
# This test is used to generate information about program status for the
# profiling report.
#
# This Makefile starts with Output/%.linked.rbc.  This is the entire input
# program linked into a single .bc file, with no optimizations.  It then
# compiles the program with profiling instrumentation (to generate profiling
# data), compiles the program using profile guided optimization and finally
# reruns the optimized program.
#
##===----------------------------------------------------------------------===##

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

REPORTS_TO_GEN := prof-edge-gen.compile \
                  prof-edge-use.compile \
                  prof-edge-use.exec \
                  vanilla.exec
REPORTS_SUFFIX := $(addsuffix .report.txt, $(REPORTS_TO_GEN))

PROGRAMS_TO_TEST_PROF := $(addsuffix .prof-edge, $(PROGRAMS_TO_TEST))
PROGRAMS_TO_TEST_PROFGEN := $(addsuffix -gen, $(PROGRAMS_TO_TEST_PROF))
PROGRAMS_TO_TEST_PROFUSE := $(addsuffix -use, $(PROGRAMS_TO_TEST_PROF))
PROGRAMS_TO_TEST_PROFVAN := $(addsuffix .vanilla, $(PROGRAMS_TO_TEST))
PROGRAMS_TO_TEST_ALLUSE := $(PROGRAMS_TO_TEST_PROFUSE) \
                           $(PROGRAMS_TO_TEST_PROFVAN)
PROGRAMS_TO_TEST_PROFALL := $(PROGRAMS_TO_TEST_PROFGEN) \
                            $(PROGRAMS_TO_TEST_ALLUSE)

# What optimizations to run before and after the insertion of profiling
# counters.
PREPROF_OPTS :=
POSTPROF_OPTS := -std-compile-opts

# What optimizations to run before and after loading profiling data.
# PRELOAD_OPTS should really be the same as PREPROF_OPTS, it isn't compulsory
# but the profiling data will not match if the control flow graphs are
# different.  POSTLOAD_OPTS can be completely different from POSTPROF_OPTS if
# desired (possibly, -O[1-9] can insert some early passes).
PRELOAD_OPTS := $(PREPROF_OPTS)
POSTLOAD_OPTS := $(POSTPROF_OPTS)

# These targets just copy files from general rules to specific names so as more
# specific rules can exploit the general ones.
$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-use.linked.rbc): \
Output/%.prof-edge-use.linked.rbc: Output/%.linked.rbc
	$(CP) $< $@

$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-gen.linked.rbc): \
Output/%.prof-edge-gen.linked.rbc: Output/%.linked.rbc
	$(CP) $< $@

$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-gen.out-nat): \
Output/%.prof-edge-gen.out-nat: Output/%.out-nat
	$(CP) $< $@
	$(CP) $<.time $@.time

$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-use.out-nat): \
Output/%.prof-edge-use.out-nat: Output/%.out-nat
	$(CP) $< $@
	$(CP) $<.time $@.time

$(PROGRAMS_TO_TEST:%=Output/%.vanilla.out-nat): \
Output/%.vanilla.out-nat: Output/%.out-nat
	$(CP) $< $@
	$(CP) $<.time $@.time

# Instrument a program with profiling counters.
$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-gen.linked.bc): \
Output/%.prof-edge-gen.linked.bc: Output/%.linked.rbc $(LOPT)
	$(VERB) $(RM) -f $(CURDIR)/$@.info
	$(RUNSAFELYLOCAL) /dev/null $@.opt \
	    $(LOPT) \
			$(PREPROF_OPTS) -insert-edge-profiling $(POSTPROF_OPTS) \
			-info-output-file=$(CURDIR)/$@.info \
			$(STATS) $(EXTRA_LOPT_OPTIONS) $< -o $@

# Optimize a program using profiling data.
$(PROGRAMS_TO_TEST_PROF:%=Output/%-use.linked.bc): \
Output/%-use.linked.bc: Output/%-use.linked.rbc Output/%-gen.prof-data $(LOPT)
	$(VERB) $(RM) -f $(CURDIR)/$@.info
	$(RUNSAFELYLOCAL) /dev/null $@.opt \
	    $(LOPT) \
			$(PRELOAD_OPTS) \
			-profile-file=Output/$*-gen.prof-data -profile-metadata-loader \
			$(POSTLOAD_OPTS) \
			-info-output-file=$(CURDIR)/$@.info \
			$(STATS) $(EXTRA_LOPT_OPTIONS) $< -o $@

# Optimize a program without profiling data.
$(PROGRAMS_TO_TEST:%=Output/%.vanilla.linked.bc): \
Output/%.vanilla.linked.bc: Output/%.linked.rbc $(LOPT)
	$(VERB) $(RM) -f $(CURDIR)/$@.info
	$(RUNSAFELYLOCAL) /dev/null $@.opt \
	    $(LOPT) \
			$(PRELOAD_OPTS) $(POSTLOAD_OPTS) \
			-info-output-file=$(CURDIR)/$@.info \
			$(STATS) $(EXTRA_LOPT_OPTIONS) $< -o $@

# Apply standard link-time optimizations to any program.
$(PROGRAMS_TO_TEST_PROFALL:%=Output/%.llvm.bc): \
Output/%.llvm.bc: Output/%.linked.bc $(LOPT)
	$(RUNSAFELYLOCAL) /dev/null $@.opt \
	    $(LOPT) \
			-std-link-opts \
			-info-output-file=$(CURDIR)/$@.info \
			$(STATS) $< $(EXTRA_LINKTIME_OPT_FLAGS) -o $@

# Compile LLVM IR to target-specific object code.
$(PROGRAMS_TO_TEST_PROFALL:%=Output/%.o): \
Output/%.o: Output/%.llvm.bc $(LLC)
	$(VERB) $(RM) -f $(CURDIR)/$@.info
	$(RUNSAFELYLOCAL) /dev/null $@.compile \
		$(LLC) $(LLCFLAGS) -filetype=obj  $< -o $@ \
			-info-output-file=$(CURDIR)/$@.info $(STATS)

# Compile LLVM IR to target-specific assembly.
$(PROGRAMS_TO_TEST_PROFALL:%=Output/%.s): \
Output/%.s: Output/%.llvm.bc $(LLC)
	$(VERB) $(RM) -f $(CURDIR)/$@.info
	$(RUNSAFELYLOCAL) /dev/null $@.compile \
		$(LLC) $(LLCFLAGS) $< -o $@ -info-output-file=$(CURDIR)/$@.info $(STATS)

ifdef TEST_INTEGRATED_ASSEMBLER

# Assemble/link a program (linking against the profiling runtime).
$(PROGRAMS_TO_TEST_PROFGEN:%=Output/%): \
Output/%: Output/%.o
	-$(CP) $<.compile.time $@.compile.time
	-$(PROGRAMLD) $< -o $@ $(LLCLIBS) $(LLCASSEMBLERFLAGS) $(X_TARGET_FLAGS) \
		$(LDFLAGS) $(LIBPROFILESO)

# Assemble/link a program.
$(PROGRAMS_TO_TEST_ALLUSE:%=Output/%): \
Output/%: Output/%.o
	-$(CP) $<.compile.time $@.compile.time
	-$(PROGRAMLD) $< -o $@ $(LLCLIBS) $(LLCASSEMBLERFLAGS) $(X_TARGET_FLAGS) \
		$(LDFLAGS)

else

# Assemble/link a program (linking against the profiling runtime).
$(PROGRAMS_TO_TEST_PROFGEN:%=Output/%): \
Output/%: Output/%.s
	-$(CP) $<.compile.time $@.compile.time
	-$(PROGRAMLD) $< -o $@ $(LLCLIBS) $(LLCASSEMBLERFLAGS) $(X_TARGET_FLAGS) \
		$(LDFLAGS) $(LIBPROFILESO)

# Assemble/link a program.
$(PROGRAMS_TO_TEST_ALLUSE:%=Output/%): \
Output/%: Output/%.s
	-$(CP) $<.compile.time $@.compile.time
	-$(PROGRAMLD) $< -o $@ $(LLCLIBS) $(LLCASSEMBLERFLAGS) $(X_TARGET_FLAGS) \
		$(LDFLAGS)

endif

# Run the instrumented program to generate profiling data.
$(PROGRAMS_TO_TEST_PROFGEN:%=Output/%.out-pgo): \
Output/%.out-pgo: Output/%
	$(VERB) $(RM) -f Output/$*.prof-data.tmp
	LLVMPROF_OUTPUT="Output/$*.prof-data.tmp" \
		$(RUNSAFELY) $(STDIN_FILENAME) $@ $< $(RUN_OPTIONS)
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

$(PROGRAMS_TO_TEST_PROFGEN:%=Output/%.prof-data): \
Output/%.prof-data: Output/%.out-pgo
	-$(CP) $@.tmp $@

# Run a non-instrumented program.
$(PROGRAMS_TO_TEST_ALLUSE:%=Output/%.out-pgo): \
Output/%.out-pgo: Output/%
	$(RUNSAFELY) $(STDIN_FILENAME) $@ $< $(RUN_OPTIONS)
ifdef PROGRAM_OUTPUT_FILTER
	$(PROGRAM_OUTPUT_FILTER) $@
endif

# Determine whether the program exectuted successfully (i.e. produced the same
# output as the reference).
$(PROGRAMS_TO_TEST_PROFALL:%=Output/%.exec): \
Output/%.exec: Output/%.out-pgo Output/%.out-nat
	-$(CP) $<.time $@.time
	$(DIFFPROG) pgo $* $(HIDEDIFF) &> $@
	@-if test `cat $@ | wc -l` -eq 0 ; then \
	  touch $@.success ;\
	else \
	  touch $@.fail ;\
	fi

$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-gen.compile.report.txt): \
Output/%.prof-edge-gen.compile.report.txt: Output/%.prof-edge-gen.exec Output/%.prof-edge-gen.linked.bc
	@echo > $@
	@-if test -f Output/$*.prof-edge-gen; then \
	  echo "TEST-PASS: prof-edge-gen-compile $(RELDIR)/$*" >> $@; \
	  echo "TEST-RESULT-prof-edge-gen-compile-success: pass" >> $@;\
	else \
	  echo "TEST-FAIL: prof-edge-gen-compile $(RELDIR)/$*" >> $@; \
	fi
	@-printf "TEST-RESULT-prof-edge-gen-compile-time: " >> $@
	@-grep "^user" Output/$*.prof-edge-gen.compile.time >> $@
	@-printf "TEST-RESULT-prof-edge-gen-compile-edges-inserted: " >> $@
	@-cat Output/$*.prof-edge-gen.linked.bc.info \
	    | grep insert-edge-profiling \
	    | grep 'The # of edges inserted.' \
	    | $(SED) 's/ *\([0-9]*\) insert-edge-profiling.*/\1/' >> $@
	@-printf "TEST-RESULT-prof-edge-gen-exec-time: " >> $@
	@-grep "^user" $<.time >> $@

$(PROGRAMS_TO_TEST:%=Output/%.prof-edge-use.compile.report.txt): \
Output/%.prof-edge-use.compile.report.txt: Output/%.prof-edge-use.exec
	@echo > $@
	@-if test -f Output/$*.prof-edge-use ; then \
	  echo "TEST-PASS: prof-edge-use-compile $(RELDIR)/$*" >> $@; \
	  echo "TEST-RESULT-prof-edge-use-compile-success: pass" >> $@;\
	else \
	  echo "TEST-FAIL: prof-edge-use-compile $(RELDIR)/$*" >> $@; \
	fi
	@-printf "TEST-RESULT-prof-edge-use-compile-time: " >> $@
	@-grep "^user" Output/$*.prof-edge-use.compile.time >> $@
	@-printf "TEST-RESULT-prof-edge-use-compile-terms-annotated: " >> $@
	@-if test `  cat Output/$*.prof-edge-use.linked.bc.info \
	           | grep profile-metadata-loader \
	           | grep 'The # of terminator instructions annotated.' \
	           | wc -l` -gt 0; \
	then \
	  cat Output/$*.prof-edge-use.linked.bc.info \
	      | grep profile-metadata-loader \
	      | grep 'The # of terminator instructions annotated.' \
	      | $(SED) 's/ *\([0-9]*\) profile-metadata-loader.*/\1/' >> $@; \
	else \
	  echo "0" >> $@; \
	fi
	@-printf "TEST-RESULT-prof-edge-use-compile-lower-expects: " >> $@
	@-if test `  cat Output/$*.prof-edge-gen.linked.bc.info \
	           | grep lower-expect-intrinsic \
	           | wc -l` -gt 0; \
	then \
	  cat Output/$*.prof-edge-gen.linked.bc.info \
	    | grep lower-expect-intrinsic \
	    | $(SED) 's/ *\([0-9]*\) lower-expect-intrinsic.*/\1/' >> $@;\
	else \
	  echo "0" >> $@; \
	fi

$(PROGRAMS_TO_TEST_ALLUSE:%=Output/%.exec.report.txt): \
Output/%.exec.report.txt: Output/%.exec
	@echo > $@
	@-is_xfail=0; \
	for i in $(EXEC_XFAILS); do \
	   if test "$*" == $$i; then \
	     is_xfail=1; \
	   fi; \
	done; \
	type="`echo $* | sed -e 's/.*\.\([a-z\-]*\)/\1/'`";\
	if test $$is_xfail == 1; then \
	  echo "TEST-XFAIL: `echo $$type`-exec $(RELDIR)/$*" >> $@;\
	  echo "TEST-RESULT-`echo $$type`-exec-success: xfail" >> $@;\
	elif test -f Output/$*.exec.success; then \
	  echo "TEST-PASS: `echo $$type`-exec $(RELDIR)/$*" >> $@;\
	  echo "TEST-RESULT-`echo $$type`-exec-success: pass" >> $@;\
	else \
	  echo "TEST-FAIL: `echo $$type`-exec $(RELDIR)/$*" >> $@;\
	fi;\
	printf "TEST-RESULT-`echo $$type`-exec-time: " >> $@
	@-grep "^user" Output/$*.exec.time >> $@
	if test -f Output/$*.extra-results.txt; then \
	  $(PROGDIR)/ParseMultipleResults $(RELDIR)/$* \
	      Output/$*.extra-results.txt >> $@; \
	fi

# Overall tests: just run subordinate tests
$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt): \
Output/%.$(TEST).report.txt: $(addprefix Output/%., $(REPORTS_SUFFIX))
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	-cat $(addprefix Output/$*., $(REPORTS_SUFFIX)) >> $@

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@-cat $<

$(PROGRAMS_TO_TEST_PROFALL:%=build.$(TEST).%): \
build.$(TEST).%: Output/%
	@echo "Finished Building: $<"
