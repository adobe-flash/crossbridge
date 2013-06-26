##===- TEST.nightly.Makefile ------------------------------*- Makefile -*--===##
#
# This test is used in conjunction with the llvm/utils/NightlyTest* stuff to
# generate information about program status for the nightly report.
#
##===----------------------------------------------------------------------===##

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

REPORTS_TO_GEN := nat
REPORT_DEPENDENCIES :=
ifndef DISABLE_LLC
REPORTS_TO_GEN +=  llc compile
REPORT_DEPENDENCIES += $(LLC) $(LOPT)
endif
ifndef DISABLE_JIT
REPORTS_TO_GEN +=  jit compile
REPORT_DEPENDENCIES += $(LLI) $(LOPT)
endif
ifdef ENABLE_LLCBETA
REPORTS_TO_GEN += llc-beta compile
REPORT_DEPENDENCIES += $(LLC) $(LOPT)
endif
ifdef ENABLE_OPTBETA
REPORTS_TO_GEN += opt-beta compile
REPORT_DEPENDENCIES += $(LOPT)
endif
REPORTS_SUFFIX := $(addsuffix .report.txt, $(REPORTS_TO_GEN))

# Compilation tests
$(PROGRAMS_TO_TEST:%=Output/%.nightly.compile.report.txt): \
Output/%.nightly.compile.report.txt: Output/%.llvm.bc
	@echo > $@
	@-if test -f Output/$*.linked.bc.info; then \
	  echo "TEST-PASS: compile $(RELDIR)/$*" >> $@;\
	  printf "TEST-RESULT-compile: " >> $@;\
	  grep "Total Execution Time" Output/$*.linked.bc.info | tail -n 1 >> $@;\
	  echo >> $@;\
	  printf "TEST-RESULT-compile: " >> $@;\
	  wc -c $< >> $@;\
	  echo >> $@;\
	else \
	  echo "TEST-FAIL: compile $(RELDIR)/$*" >> $@;\
	fi

# NAT tests
$(PROGRAMS_TO_TEST:%=Output/%.nightly.nat.report.txt): \
Output/%.nightly.nat.report.txt: Output/%.out-nat
	@echo > $@
	@printf "TEST-RESULT-nat-time: " >> $@
	-grep "^user" Output/$*.out-nat.time >> $@

# LLC tests
$(PROGRAMS_TO_TEST:%=Output/%.nightly.llc.report.txt): \
Output/%.nightly.llc.report.txt: Output/%.exe-llc
	@echo > $@
	@-if test -f Output/$*.exe-llc; then \
	  head -n 100 Output/$*.exe-llc >> $@; \
	  echo "TEST-PASS: llc $(RELDIR)/$*" >> $@;\
	  printf "TEST-RESULT-llc: " >> $@;\
	  grep "Total Execution Time" Output/$*.llc.s.info | tail -n 1 >> $@;\
	  printf "TEST-RESULT-llc-time: " >> $@;\
	  grep "^user" Output/$*.out-llc.time >> $@;\
	  echo >> $@;\
	else  \
	  echo "TEST-FAIL: llc $(RELDIR)/$*" >> $@;\
	fi

# LLC experimental tests
$(PROGRAMS_TO_TEST:%=Output/%.nightly.llc-beta.report.txt): \
Output/%.nightly.llc-beta.report.txt: Output/%.llvm.bc Output/%.exe-llc-beta
	@echo > $@
	@-if test -f Output/$*.exe-llc-beta; then \
	  head -n 100 Output/$*.exe-llc-beta >> $@; \
	  echo "TEST-PASS: llc-beta $(RELDIR)/$*" >> $@;\
	  printf "TEST-RESULT-llc-beta: " >> $@;\
	  grep "Total Execution Time" Output/$*.llc-beta.s.info | tail -n 1 >> $@;\
	  printf "TEST-RESULT-llc-beta-time: " >> $@;\
	  grep "^user" Output/$*.out-llc-beta.time >> $@;\
	  echo >> $@;\
	else  \
	  echo "TEST-FAIL: llc-beta $(RELDIR)/$*" >> $@;\
	fi

# OPT experimental tests
$(PROGRAMS_TO_TEST:%=Output/%.nightly.opt-beta.report.txt): \
Output/%.nightly.opt-beta.report.txt: Output/%.exe-opt-beta
	@echo > $@
	@-if test -f Output/$*.exe-opt-beta; then \
	  head -n 100 Output/$*.exe-opt-beta >> $@; \
	  echo "TEST-PASS: opt-beta $(RELDIR)/$*" >> $@;\
	  printf "TEST-RESULT-opt-beta: " >> $@;\
	  grep "Total Execution Time" Output/$*.opt-beta.s.info | tail -n 1 >> $@;\
	  printf "TEST-RESULT-opt-beta-time: " >> $@;\
	  grep "^user" Output/$*.out-opt-beta.time >> $@;\
	  echo >> $@;\
	else  \
	  echo "TEST-FAIL: opt-beta $(RELDIR)/$*" >> $@;\
	fi

# JIT tests
$(PROGRAMS_TO_TEST:%=Output/%.nightly.jit.report.txt): \
Output/%.nightly.jit.report.txt: Output/%.exe-jit
	@echo > $@
	@-if test -f Output/$*.exe-jit; then \
	  head -n 100 Output/$*.exe-jit >> $@; \
	  echo "TEST-PASS: jit $(RELDIR)/$*" >> $@;\
	  printf "TEST-RESULT-jit-time: " >> $@;\
	  grep "^user" Output/$*.out-jit.time >> $@;\
	  echo >> $@;\
	  printf "TEST-RESULT-jit-comptime: " >> $@;\
	  grep "Total Execution Time" Output/$*.out-jit.info | tail -n 1 >> $@;\
	  echo >> $@;\
	else  \
	  echo "TEST-FAIL: jit $(RELDIR)/$*" >> $@;\
	fi

# Overall tests: just run subordinate tests
$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt): \
Output/%.$(TEST).report.txt: $(addprefix Output/%.nightly., $(REPORTS_SUFFIX))
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	-cat $(addprefix Output/$*.nightly., $(REPORTS_SUFFIX)) >> $@



$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@-cat $<

ifdef ENABLE_LLCBETA
$(PROGRAMS_TO_TEST:%=build.$(TEST).%): \
build.$(TEST).%: Output/%.llc Output/%.llc-beta
	@echo "Finished Building: $<"
else
$(PROGRAMS_TO_TEST:%=build.$(TEST).%): \
build.$(TEST).%: Output/%.llc
	@echo "Finished Building: $<"
endif
