##===- TEST.simple.Makefile -------------------------------*- Makefile -*--===##
#
# This test is used in conjunction with the llvm/utils/NightlyTest* stuff to
# generate information about program status for the nightly report.
#
##===----------------------------------------------------------------------===##

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

REPORTS_TO_GEN := compile exec
REPORTS_SUFFIX := $(addsuffix .report.txt, $(REPORTS_TO_GEN))

#$(PROGRAMS_TO_TEST:%=Output/%.simple.compile.report.txt): \
#Output/%.simple.compile.report.txt: Output/%.out-simple
#	@echo > $@
#	@printf "TEST-RESULT-compile-time: " >> $@
#	-grep "^program" Output/$*.simple.compile.time >> $@

$(PROGRAMS_TO_TEST:%=Output/%.simple.compile.report.txt): \
Output/%.simple.compile.report.txt: Output/%.out-simple
	@echo > $@
	@-if test -f Output/$*.simple; then \
	  echo "TEST-PASS: compile $(RELDIR)/$*" >> $@; \
	  echo "TEST-RESULT-compile-success: pass" >> $@;\
	else \
	  echo "TEST-FAIL: compile $(RELDIR)/$*" >> $@; \
	fi
	@-printf "TEST-RESULT-compile-time: " >> $@
	@-grep "^user" Output/$*.simple.compile.time >> $@
	@-printf "TEST-RESULT-compile-real-time: " >> $@
	@-grep "^real" Output/$*.simple.compile.time >> $@

$(PROGRAMS_TO_TEST:%=Output/%.simple.exec.report.txt): \
Output/%.simple.exec.report.txt: Output/%.exe-simple
	@echo > $@
	@-is_xfail=0; \
	for i in $(EXEC_XFAILS); do \
	   if test "$*" == $$i; then \
	     is_xfail=1; \
	   fi; \
	done; \
	if test $$is_xfail == 1; then \
	  echo "TEST-XFAIL: exec $(RELDIR)/$*" >> $@;\
	  echo "TEST-RESULT-exec-success: xfail" >> $@;\
	elif test -f Output/$*.exe-simple; then \
	  echo "TEST-PASS: exec $(RELDIR)/$*" >> $@;\
	  echo "TEST-RESULT-exec-success: pass" >> $@;\
	else  \
	  echo "TEST-FAIL: exec $(RELDIR)/$*" >> $@;\
	fi
	@-printf "TEST-RESULT-exec-time: " >> $@
	@-grep "^user" Output/$*.out-simple.time >> $@
	@-printf "TEST-RESULT-exec-real-time: " >> $@
	@-grep "^real" Output/$*.out-simple.time >> $@
	if test -f Output/$*.extra-results.txt; then \
	  $(PROGDIR)/ParseMultipleResults $(RELDIR)/$* Output/$*.extra-results.txt >> $@; \
	fi

# Overall tests: just run subordinate tests
$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt): \
Output/%.$(TEST).report.txt: $(addprefix Output/%.simple., $(REPORTS_SUFFIX))
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	-cat $(addprefix Output/$*.simple., $(REPORTS_SUFFIX)) >> $@

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@-cat $<

$(PROGRAMS_TO_TEST:%=build.$(TEST).%): \
build.$(TEST).%: Output/%.simple
	@echo "Finished Building: $<"
